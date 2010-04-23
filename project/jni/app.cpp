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

//by setting stateTransition and transitionDelay to something non-null, one can put a small delay on the state transition
//There is no guarantee that the state transition won't be overrided during this delay though
int transitionDelay = 0; 
#define WON_LOST_DELAY 500

void startGame (GameManager* manager) {
  transitionDelay = 0;
  stateTransition = STATE_PLAYING;
}

void nativeMenu () {
  transitionDelay = 0;
  stateTransition = STATE_MAINMENU;
}

void gameOverCallback () {
  //Since we're using a delay, this will be called multiple times, just discard
  if (stateTransition == STATE_LOST)
    return;
  transitionDelay = WON_LOST_DELAY;
  stateTransition = STATE_LOST;
}

void gameWonCallback () {
  //we're using a delay => this function will be called multiple times, just discard
  if (stateTransition == STATE_WON)
    return;
  transitionDelay = WON_LOST_DELAY;
  stateTransition = STATE_WON;
}

void gameUnPauseCallback () {
  transitionDelay = 0;
  stateTransition = STATE_PLAYING;
}

GameManager* gameManager = NULL;

Sprite* levelText = NULL;
//CONTROL_GAMEPAD
//CONTROL_DOUBLETAP
#define CONTROL_DOUBLETAP

#ifdef CONTROL_GAMEPAD
Sprite* gamePad = NULL;
const Vector2 gamePadPos(13.8f, 5.0f);
const Vector2 gamePadSize(2.5,2.5);
#endif

void toPlayingState () {
  if (gameManager->getState() == STATE_PAUSED) {
    game->unpause(); 
  } else {
    delete lvl;
    delete game;
    delete gameView;

    lvl = levelsLoadFns[gameManager->getCurrentLevel()]();
    game = new Game(gameOverCallback, gameWonCallback, lvl);
    gameView = new GameView(*game);

    centerGameOnScreen();
  }
  gameManager->setState(STATE_PLAYING);
}

void toMenuState (eAppState state) {
  delete lvl;
  lvl = NULL;
  delete game;
  game = NULL;
  delete gameView;
  gameView = NULL;
  gameManager->setState(state);
}

void toMainMenuState () {
  //FIXME: shouldn't we "pause" the game ?
  toMenuState(STATE_MAINMENU);
}

void toLostState () {
  toMenuState(STATE_LOST);
}

void toWonState () {
  if (gameManager->isAtLastLevel())
    toMenuState(STATE_END);
  else
    toMenuState(STATE_WON);
}

void toEndState () {
  toMenuState(STATE_END);
}

void toPauseState () {
  game->pause();
  gameManager->setState(STATE_PAUSED);
}

void nativeInit (const char* apkPath) {
  loadAPK(apkPath);
}

/** Since nativeInitGL will be called on app creation AND each time the opengl 
 * context is recreated, this indicate if we're at app creation (false) or
 * just in context recreation (true) */
bool initialised = false;


void nativeInitGL() {
  if (!initialised) {
    initialised = true;
    //This is the first time initialisation, we HAVE to instantiate 
    //game manager here because it requires textures
    gameManager = new GameManager(startGame, nativeMenu, gameUnPauseCallback);

    levelText = new Sprite("assets/sprites/level_text.png");
#ifdef CONTROL_GAMEPAD
    gamePad = new Sprite("assets/sprites/control.png");
#endif

    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);
  } else {
    //In case we're coming back from sleep
    TextureManager::getInstance()->clear();
    Sprite::reloadAllSprites();
  }

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
  const int levelW = game->getLevel()->getWidth();
  //screen size in game coords
  const float gsW = viewportWidth*xScreenToGame;
  const float gsH = viewportHeight*yScreenToGame;
#ifdef CONTROL_EDGES
  transX = 0.5f + (gsW-levelW)/2.0f;
  transY = 0.5f + (gsH-levelH)/2.0f;
#elif defined(CONTROL_GAMEPAD)
  transX = 1.0f;
  transY = 0.5f + (gsH-levelH)/2.0f;

  game->setGamePadPos(gamePadPos - Vector2(transX, transY));
#else
  transX = 1.0f;
  transY = 0.5f + (gsH-levelH)/2.0f;
#endif
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

void nativePause () {
  if (gameManager->getState() == STATE_PLAYING) {
    transitionDelay = 0;
    stateTransition = STATE_PAUSED;
  }
}

static uint64_t last = Utils::getCurrentTimeMillis();

void nativeRender () {
  uint64_t now = Utils::getCurrentTimeMillis();
  if (stateTransition != -1) {
    transitionDelay -= (int)(now-last);
    if (transitionDelay <= 0) {
      switch (stateTransition) {
        case STATE_PLAYING: toPlayingState(); break;
        case STATE_MAINMENU: toMainMenuState(); break;
        case STATE_LOST: toLostState(); break;
        case STATE_WON: toWonState(); break;
        case STATE_END: toEndState(); break;
        case STATE_PAUSED: toPauseState(); break;
      }
      stateTransition = -1;
      transitionDelay = 0;
    }
  }
  last = now;

  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();

  if (gameManager->inGame() || gameManager->paused()) {
    if (!gameManager->paused())
      game->update();

    //levelText->draw(Vector2(13.7f, 2.0f), Vector2(3.0f,3.0f));
#ifdef CONTROL_GAMEPAD
    gamePad->draw(gamePadPos, gamePadSize);
#endif

    glPushMatrix();
    GLW::translate(10.0f, 0.55f, 0);
    gameView->drawHearts();
    glPopMatrix();

    glPushMatrix();
    GLW::translate(transX, transY, 0);
    gameView->draw();
    //gameView->debugDraw();
    glPopMatrix();
    
    if (gameManager->paused())
      gameManager->drawMenu(); //draw the pause menu
  } else {
    gameManager->drawMenu();
  }
}

#ifdef CONTROL_GAMEPAD
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
#endif


#ifdef CONTROL_EDGES
enum eScreenEdges {
    EDGE_TOP,
    EDGE_TOP_RIGHT,
    EDGE_RIGHT,
    EDGE_BOTTOM_RIGHT,
    EDGE_BOTTOM,
    EDGE_BOTTOM_LEFT,
    EDGE_LEFT,
    EDGE_TOP_LEFT,
    EDGE_NONE
};

eScreenEdges inEdge (float x, float y) {
  const float minx = transX;
  const float miny = transY;
  const float maxx = viewportWidth*xScreenToGame - transX;
  const float maxy = viewportHeight*yScreenToGame - transY;

  //x position relative to [minx,maxx] => -1 is on the left, 0 inside and 1 on the right
  const int xpos = (x < minx)?-1:(x<maxx)?0:1;
  const int ypos = (y < miny)?-1:(y<maxy)?0:1;

  //LOGE("inEdge (%f,%f), min (%f,%f) max (%f,%f) => x/y : (%i,%i)", x, y, minx, miny, maxx, maxy, xpos, ypos);

  if (ypos == -1) {
    if (xpos == -1) return EDGE_TOP_LEFT;
    if (xpos == 0) return EDGE_TOP;
    if (xpos == 1) return EDGE_TOP_RIGHT;
  } else if (ypos == 0) {
    if (xpos == -1) return EDGE_LEFT;
    if (xpos == 0) return EDGE_NONE;
    if (xpos == 1) return EDGE_RIGHT;
  } else if (ypos == 1) {
    if (xpos == -1) return EDGE_BOTTOM_LEFT;
    if (xpos == 0) return EDGE_BOTTOM;
    if (xpos == 1) return EDGE_BOTTOM_RIGHT;
  }
  ASSERT(false);
  return EDGE_NONE;
}

Vector2 edgeMoveDir (eScreenEdges edge) {
  switch (edge) {
    case EDGE_TOP_LEFT: return Vector2(-1,-1);
    case EDGE_TOP: return Vector2(0, -1);
    case EDGE_TOP_RIGHT: return Vector2(1, -1);
    case EDGE_LEFT: return Vector2(-1, 0);
    case EDGE_NONE: return Vector2(0, 0);
    case EDGE_RIGHT : return Vector2(0, 1);
    case EDGE_BOTTOM_LEFT: return Vector2(-1, 1);
    case EDGE_BOTTOM: return Vector2(0, 1);
    case EDGE_BOTTOM_RIGHT: return Vector2(1, 1);
    default: ASSERT(false);
  }
}
#endif



/**
 * See important comment in Moob.java regarding native event-handling methods.
 * Basically, they should lead to any OpenGL call, because they'll be run in a separate
 * thread than the rendering thread.
 */

static uint64_t lastTouchDownTime = 0;
static Vector2 lastTouchDownLocation;

void touchEventDown (float x, float y) {
  if (!gameManager->inGame()) {
    //LOGE("touchEventDown(menu) (%f,%f) => (%f,%f)", x, y, XSG_NOTRANSX(x), YSG_NOTRANSY(y));
    gameManager->handleTouchDown(Vector2(XSG_NOTRANSX(x), YSG_NOTRANSY(y)));
    return;
  }

#ifdef CONTROL_DOUBLETAP
  const Vector2 p(XSG(x), YSG(y));
  //Check that the double-tap was quick and not too far away (which would indicate a fast direction
  //change)
  const uint64_t now = Utils::getCurrentTimeMillis();
  const float tapDist = (p-lastTouchDownLocation).length();
  const uint64_t elapsed = now - lastTouchDownTime;
  LOGE("time between taps : %li, dist between tap : %f", (long)(now-lastTouchDownTime), tapDist);
  if (tapDist < 1 && elapsed <= 200) {
    //FIXME: this is ugly =)
    game->startMoving(MOVING_CURSOR, p);
    game->stopMoving();
    game->startMoving(MOVING_TANK, p);
  } else {
    game->startMoving(MOVING_TANK, p);
  }
  lastTouchDownTime = now;
  lastTouchDownLocation = p;
  return;
#endif

  //LOGE("touchEventDown(inGame) (%f,%f) => (%f,%f)", x, y, p.x, p.y);
#ifdef CONTROL_EDGES
  const eScreenEdges e = inEdge(XSG_NOTRANSX(x),YSG_NOTRANSY(y));
  if (e != EDGE_NONE) {
    game->startMoving(MOVING_TANK, p);
  } else
#endif
#ifdef CONTROL_GAMEPAD
  if (inGamePad(x,y))
    game->startMoving(MOVING_TANK_PAD, p);
  else
#endif
  if (gameView->getTankView().touchInside(p))
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

