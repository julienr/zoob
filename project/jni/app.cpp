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


//Since startGame/nativeMenu cannot call openGL, this is used to defer state change
//If this variable is not set to -1, then a state transition to (eAppState)stateTransition
//must be made before next rendering
int stateTransition = -1;

void startGame (GameManager* manager) {
  stateTransition = STATE_PLAYING;
}

void nativeMenu () {
  stateTransition = STATE_MAINMENU;
}

void gameOverCallback () {
  LOGE("gameover");
  stateTransition = STATE_LOST;
}

GameManager* gameManager;

Sprite* levelText;
Sprite* gamePad;
const Vector2 gamePadPos(13.8f, 5.0f);
const Vector2 gamePadSize(2.5,2.5);

void toPlayingState (GameManager* manager) {
  delete lvl;
  delete game;
  delete gameView;

  lvl = levelsLoadFns[manager->getCurrentLevel()]();
  game = new Game(gameOverCallback, lvl);
  gameView = new GameView(*game);

  centerGameOnScreen();
}

void toMainMenuState () {
  //FIXME: shouldn't we "pause" the game ?
  delete lvl;
  lvl = NULL;
  delete game;
  game = NULL;
  delete gameView;
  gameView = NULL;
  gameManager->setState(STATE_MAINMENU);
}

void toLostState () {
  delete lvl;
  lvl = NULL;
  delete game;
  game = NULL;
  delete gameView;
  gameView = NULL;
  gameManager->setState(STATE_LOST);
}


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
#define XSG(x) (x*xScreenToGame-(transX+0.5))
#define YSG(y) (y*yScreenToGame-(transY+0.5))

#define XSG_NOTRANSX(x) (x*xScreenToGame-0.5)
#define YSG_NOTRANSY(y) (y*yScreenToGame-0.5)

int windowWidth = 0, windowHeight = 0;
int viewportWidth = 0, viewportHeight = 0;

void centerGameOnScreen () {
  //Center game area on screen
  const int levelH = game->getLevel()->getHeight();
  //const int levelW = game->getLevel()->getWidth();
  //screen size in game coords
  //const float gsW = screenWidth*xScreenToGame;
  const float gsH = viewportHeight*yScreenToGame;
  /*transX = 0.5f + (gsW-levelW)/2.0f;*/
  transX = 1.0f;
  transY = 0.5f + (gsH-levelH)/2.0f;

  game->setGamePadPos(gamePadPos - Vector2(transX, transY));
}

void forceRatio (float sW, float sH) {
/**
  * We base all our calculations on a 480/320 = 1.5 aspect ratio (that's the most
  * common resolution for android and iphone)
  * If we have a screen that doesn't have this aspect ratio, we use the biggest square
  * that has this aspect ratio that we can display in this screen and we display the menu
  * in this square.
  * This will of course waste some screen space, but at least the menu will look good
  */
  const float targetRatio = 1.5f;
  float ratio = sW/sH;
  //The subarea in which we'll display our objects
  float areaHeight = sH;
  float areaWidth = sW;
  if (!Math::epsilonEq(ratio, targetRatio)) {
   if (sH > sW/1.5) {
     areaHeight = sW/1.5;
     areaWidth = sW;
   } else {
     areaWidth = sH*1.5;
     areaHeight = sH;
   }
  }
  ratio = areaWidth/areaHeight;
  LOGE("ratio=%f\tsW=%f, sH=%f, areaWidth=%f, areaHeight=%f", ratio, sW, sH, areaWidth, areaHeight);
  ASSERT(Math::epsilonEq(ratio, targetRatio));

  //Force the viewport to the top-left of the window
  glViewport(0, sH-areaHeight, areaWidth, areaHeight);
  GLW::checkError("glViewport");
  viewportWidth = areaWidth;
  viewportHeight = areaHeight;
}

void nativeResize (int w, int h) {
  LOGI("nativeResize (%i,%i)", w, h);
  if(h == 0)
    h = 1;
  windowWidth = w;
  windowHeight = h;
  forceRatio(w, h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //We work with a forced 1.5 aspect ratio => [15,10] game area
  const float gameAreaW = 15;
  const float gameAreaH = 10;
  GLW::ortho(0, gameAreaW, gameAreaH, 0, -1, 1);

  xScreenToGame = gameAreaW/viewportWidth;
  yScreenToGame = gameAreaH/viewportHeight;

  if (gameManager->inGame())
    centerGameOnScreen();

  glMatrixMode(GL_MODELVIEW);
}

void nativeRender () {
  if (stateTransition != -1) {
    if (stateTransition == STATE_PLAYING)
      toPlayingState(gameManager);
    else if (stateTransition == STATE_MAINMENU)
      toMainMenuState();
    else if (stateTransition == STATE_LOST)
      toLostState();
    stateTransition = -1;
  }

  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();

  if (gameManager->inGame()) {
    game->update();

    //levelText->draw(Vector2(13.7f, 2.0f), Vector2(3.0f,3.0f));
    gamePad->draw(gamePadPos, gamePadSize);

    glPushMatrix();
    GLW::translate(10.0f, 0.55f, 0);
    gameView->drawHearts();
    glPopMatrix();

    GLW::translate(transX, transY, 0);
    gameView->draw();
    gameView->debugDraw();
  } else {
    gameManager->drawMenu();
  }
}

bool inGamePad (float x, float y) {
  x *= xScreenToGame;
  y *= yScreenToGame;
  x -= 0.5;
  y -= 0.5;

  const float hW = gamePadSize.x/2.0f;
  const float hH = gamePadSize.y/2.0f;

  return ((x >= gamePadPos.x-hW) && (x <= gamePadPos.x + hW)) &&
          ((y >= gamePadPos.y-hW) && (y <= gamePadPos.y + hH));
}


/**
 * See important comment in Moob.java regarding native event-handling methods.
 * Basically, they should lead to any OpenGL call, because they'll be run in a separate
 * thread than the rendering thread.
 */

void touchEventDown (float x, float y) {
  if (!gameManager->inGame()) {
    //LOGE("touchEventDown(menu) (%f,%f) => (%f,%f)", x, y, XSG_NOTRANSX(x), YSG_NOTRANSY(y));
    gameManager->handleTouchDown(Vector2(XSG_NOTRANSX(x), YSG_NOTRANSY(y)));
    return;
  }

  const Vector2 p(XSG(x), YSG(y));
  //LOGE("touchEventDown(inGame) (%f,%f) => (%f,%f)", x, y, p.x, p.y);
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

