#include "def.h"
#include "view/TextureManager.h"
#include "logic/Entity.h"
#include "view/TankView.h"
#include "logic/Tank.h"
#include "logic/Level.h"
#include "view/LevelView.h"
#include "view/GameView.h"
#include "logic/Game.h"
#include "view/GLW.h"
#include "view/GameManager.h"

zip* APKArchive;

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGE("after %s() glError (0x%x)\n", op, error);
    }
}

static void loadAPK (const char* apkPath) {
  LOGI("Loading APK %s", apkPath);
  APKArchive = zip_open(apkPath, 0, NULL);
  if (APKArchive == NULL) {
    LOGE("Error loading APK");
    return;
  }

  //Just for debug, print APK contents
  int numFiles = zip_get_num_files(APKArchive);
  for (int i=0; i<numFiles; i++) {
    const char* name = zip_get_name(APKArchive, i, 0);
    if (name == NULL) {
      LOGE("Error reading zip file name at index %i : %s", i, zip_strerror(APKArchive));
      return;
    }
    LOGI("File %i : %s\n", i, name);
  }
}

void centerGameOnScreen ();
#include "levels/LevelsData.h"
Level* lvl = NULL;
Game* game = NULL;
GameView* gameView = NULL;

//Callback for when a new game starts, to be called ONLY by gameManager
void startGame (GameManager* manager) {
  delete lvl;
  delete game;
  delete gameView;

  lvl = levelsLoadFns[manager->getCurrentLevel()]();
  game = new Game(lvl);
  gameView = new GameView(*game);

  centerGameOnScreen();
}


GameManager* gameManager;

Sprite* levelText;
Sprite* gamePad;
const Vector2 gamePadPos(13.7f, 6.0f);
const Vector2 gamePadSize(2.3,2.3);


void nativeInit (const char* apkPath) {
  loadAPK(apkPath);

  gameManager = new GameManager(&startGame);

  levelText = new Sprite("assets/sprites/level_text.png");
  gamePad = new Sprite("assets/sprites/control.png");

  printGLString("Version", GL_VERSION);
  printGLString("Vendor", GL_VENDOR);
  printGLString("Renderer", GL_RENDERER);
  printGLString("Extensions", GL_EXTENSIONS);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnable(GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_TEXTURE_2D);
  //glClearColor(1,1,1,1);
  glClearColor(0.4f, 0.4f, 0.4f, 1);
  glColor4f(1,1,1,1);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
}

void nativeQuit () {
  TextureManager::destroy();
  delete lvl;
  delete game;
  delete gameView;
}

/** OpenGL ES doesn't necessarily support retrieving current projection/viewport matrix
 * => We do it by hand cause we now we're in 2d, so that's pretty easy
 * To convert x (screen coords) to game coords, simply do x*xScreenToGame
 */
float xScreenToGame;
float yScreenToGame;

//The margin on (x,y) on each side of the game area, for rendering
float transX = 0;
float transY = 0;

//0.5 is because sprites are square centered on their position
#define XSG(x) (x*xScreenToGame-(transX))
#define YSG(y) (y*yScreenToGame-(transY))

#define XSG_NOTRANSX(x) (x*xScreenToGame-0.5)
#define YSG_NOTRANSY(y) (y*yScreenToGame-0.5)

int screenWidth = 0, screenHeight = 0;

void centerGameOnScreen () {
  //Center game area on screen
  const int levelH = game->getLevel()->getHeight();
  const int levelW = game->getLevel()->getWidth();
  //screen size in game coords
  const float gsW = screenWidth*xScreenToGame;
  const float gsH = screenHeight*yScreenToGame;
  /*transX = 0.5f + (gsW-levelW)/2.0f;*/
  transX = 1.0f;
  transY = 0.5f + (gsH-levelH)/2.0f;

  game->setGamePadPos(gamePadPos - Vector2(transX, transY));
}

void nativeResize (int w, int h) {
  LOGI("nativeResize (%i,%i)", w, h);
  if(h == 0)
    h = 1;
  screenWidth = w;
  screenHeight = h;
  glViewport(0, 0, w, h);
  checkGlError("glViewport");
  glMatrixMode(GL_PROJECTION);
  const float ratio=w/(float)h;
  glLoadIdentity();
  const float gameAreaW = 15;
  const float gameAreaH = 15/ratio;
  GLW::ortho(0, gameAreaW, gameAreaH, 0, -1, 1);

  xScreenToGame = gameAreaW/w;
  yScreenToGame = gameAreaH/h;

  if (gameManager->inGame())
    centerGameOnScreen();

  gameManager->resize(screenWidth*xScreenToGame, screenHeight*yScreenToGame);

  checkGlError("glViewport");
  glMatrixMode(GL_MODELVIEW);
}

void nativeRender () {
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();

  if (gameManager->inGame()) {
    levelText->draw(Vector2(13.7f, 2.0f), Vector2(3.0f,3.0f));
    gamePad->draw(gamePadPos, gamePadSize);

    GLW::translate(transX, transY, 0);

    game->update();
    gameView->draw();
    //gameView->debugDraw();
  } else {
    gameManager->drawMenu();
  }
}

void nativeMenu () {
  //FIXME: shouldn't we "pause" the game ?
  delete lvl;
  lvl = NULL;
  delete game;
  game = NULL;
  delete gameView;
  gameView = NULL;
  gameManager->menuMode();
}

void nativePause () {
  LOGE("Pause");
}

bool inGamePad (float x, float y) {
  x *= xScreenToGame;
  y *= yScreenToGame;

  const float hW = gamePadSize.x/2.0f;
  const float hH = gamePadSize.y/2.0f;

  return ((x >= gamePadPos.x-hW) && (x <= gamePadPos.x + hW)) &&
          ((y >= gamePadPos.y-hW) && (y <= gamePadPos.y + hH));
}

void touchEventDown (float x, float y) {
  if (!gameManager->inGame()) {
    gameManager->handleTouchDown(Vector2(XSG_NOTRANSX(x), YSG_NOTRANSY(y)));
    return;
  }

  const Vector2 p(XSG(x), YSG(y));
  if (inGamePad(x,y))
    game->startMoving(MOVING_TANK_PAD, p);
  else if (gameView->getTankView().touchInside(p))
    game->startMoving(MOVING_TANK, p);
  else
    game->startMoving(MOVING_CURSOR, p);
}

void touchEventMove (float x, float y) {
  if (!gameManager->inGame())
    return;
  const Vector2 p(XSG(x), YSG(y));
  game->setMoveTouchPoint(p);
}

void touchEventUp (float x, float y) {
  if (gameManager->inGame())
    game->stopMoving();
  else
    gameManager->handleTouchUp(Vector2(XSG_NOTRANSX(x), YSG_NOTRANSY(y)));
}

void touchEventOther (float x, float y) {
  touchEventUp(x,y);
}

