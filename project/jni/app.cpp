#include "def.h"
#include "view/textures/TextureManager.h"
#include "logic/Entity.h"
#include "view/TankView.h"
#include "logic/Tank.h"
#include "logic/Level.h"
#include "view/LevelView.h"
#include "view/GameView.h"
#include "logic/Game.h"
#include "view/GLW.h"
#include "view/GameManager.h"
#include "input/AndroidInputManager.h"
#include "logic/Difficulty.h"
#include "view/NumberView.h"
#include "view/Square.h"


extern void saveProgress (int level);
extern void saveDifficulty (int diff);

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
  /*int numFiles = zip_get_num_files(APKArchive);
  for (int i=0; i<numFiles; i++) {
    const char* name = zip_get_name(APKArchive, i, 0);
    if (name == NULL) {
      LOGE("Error reading zip file name at index %i : %s", i, zip_strerror(APKArchive));
      return;
    }
    LOGI("File %i : %s\n", i, name);
  }*/
}

void centerGameInViewport ();
#include "levels/LevelsData.h"
Level* lvl = NULL;
GameView* gameView = NULL;


#define WON_LOST_DELAY 1000

void startGame (GameManager* manager) {
  manager->setState(STATE_PLAYING);
}

void nativeMenu () {
  GameManager::getInstance()->setState(STATE_MAINMENU);
}

void gameOverCallback () {
  GameManager::getInstance()->setState(STATE_LOST, WON_LOST_DELAY);
}

void gameWonCallback () {
  GameManager::getInstance()->setState(STATE_WON, WON_LOST_DELAY);
}

void gameUnPauseCallback () {
  GameManager::getInstance()->setState(STATE_PLAYING);
}

AndroidInputManager* inputManager = NULL;

void toPlayingState () {
  GameManager* gm = GameManager::getInstance();
  if (Game::getInstance() && Game::getInstance()->isPaused()) {
    Game::getInstance()->unpause();
  } else {
    delete lvl;
    Game::destroy();
    delete gameView;

    lvl = levelsLoadFns[GameManager::getInstance()->getCurrentLevel()]();
    Game::create(gameOverCallback, gameWonCallback, lvl);
    gameView = new GameView();
    ProgressionManager::getInstance()->changedLevel();

    inputManager->reset();

    centerGameInViewport();
    saveDifficulty(Difficulty::getInstance()->currentDifficulty());

    //Display the tutorial before starting level 0
    if (gm->getCurrentLevel() == 0) {
      Game::getInstance()->pause();
      gm->setState(STATE_TUTORIAL);
    }
  }
}

void toMenuState () {
  delete lvl;
  lvl = NULL;
  Game::destroy();
  delete gameView;
  gameView = NULL;
}


void toWonState () {
  if (!GameManager::getInstance()->hasMoreLevels()) {
    GameManager::getInstance()->setState(STATE_END);
  } else {
    toMenuState();
    saveProgress(GameManager::getInstance()->getCurrentLevel()+1);
    if (ProgressionManager::getInstance()->getLastReward() != REWARD_NONE)
       GameManager::getInstance()->setState(STATE_REWARD);
  }
}

void toPauseState () {
  Game::getInstance()->pause();
}

void nativeInit (const char* apkPath) {
  srand(Utils::getCurrentTimeMillis());
  loadAPK(apkPath);
}

/** Since nativeInitGL will be called on app creation AND each time the opengl 
 * context is recreated, this indicate if we're at app creation (false) or
 * just in context recreation (true) */
bool initialised = false;


void nativeInitGL(int level, int difficulty) {
  if (!initialised) {
    initialised = true;
    Difficulty::setDifficulty(difficulty);
    //This is the first time initialisation, we HAVE to instantiate 
    //game manager here because it requires textures
    GameManager::create(startGame, nativeMenu, gameUnPauseCallback, level);
    GameManager* gm = GameManager::getInstance();
    gm->setStateCallback(STATE_WON, toWonState);
    gm->setStateCallback(STATE_LOST, toMenuState);
    gm->setStateCallback(STATE_END, toMenuState);
    gm->setStateCallback(STATE_MAINMENU, toMenuState);
    gm->setStateCallback(STATE_PLAYING, toPlayingState);
    gm->setStateCallback(STATE_PAUSED, toPauseState);
    inputManager = new AndroidInputManager();

    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);
  } else {
    //In case we're coming back from sleep
    TextureManager::getInstance()->reloadAll();
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
  zip_close(APKArchive);
  TextureManager::destroy();
  delete lvl;
  Game::destroy();
  delete gameView;
}

void toggleGodMode () {
  if (Game::getInstance())
    Game::getInstance()->toggleGodMode();
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

//We work with a forced 1.5 aspect ratio => [15,10] viewport
#define VIEWPORT_WIDTH 15
#define VIEWPORT_HEIGHT 10

/**
 * The screen is divided as follow :
 * - the biggest are is the window
 * - then comes the viewport that contains EVERYTHING that we ever render. it is
 *   COMPLETELY FORBIDDEN to render outside the viewport since it won't be visible on some devices
 *   depending on the screen ratio
 * - then comes the game area where the actual game (level, tanks) are rendered
 * |----------------------------------|
 * |      |-----------------|         |
 * |      |    |-----| life |         |
 * |      |    | game| pad  |         |
 * |      |    |-----|      |         |
 * |      |-----------------|         |
 * |                                  |
 * |----------------------------------|
 */
//viewport margin relative to the screen (in screen coords). This should be used only for input
//since viewport placement is handled by opengl glViewport
Vector2 viewportMargin;

//0.5 is because sprites are square centered on their position
float XSG (const float x) {
  return (x-viewportMargin.x)*xScreenToGame-(transX+0.5);
}

float YSG (const float y) {
  return (y-viewportMargin.y)*yScreenToGame-(transY+0.5);
}

float XSG_NOTRANSX (const float x) {
  return (x-viewportMargin.x)*xScreenToGame-0.5;
}

float YSG_NOTRANSY (const float y) {
  return (y-viewportMargin.y)*yScreenToGame-0.5;
}

float XGS (const float x) {
  return (x+transX-0.5)/xScreenToGame + viewportMargin.x;
}

float YGS (const float y) {
  return (y+transY-0.5)/yScreenToGame + viewportMargin.y;
}

int windowWidth = 0, windowHeight = 0;

//viewport dimensions in screen size
Vector2 viewportScreenDim;

void centerGameInViewport () {
  //Center game area on screen
  const int levelH = Game::getInstance()->getLevel()->getHeight();
  //const int levelW = game->getLevel()->getWidth();

  //FIXME
  //if (showGamePad) {
    transX = 1.0f;
    transY = 0.5f + (VIEWPORT_HEIGHT-levelH)/2.0f;
  /*  game->setGamePadPos(gamePadPos - Vector2(transX, transY));
  } else {
    transX = 0.5f + (VIEWPORT_WIDTH-levelW)/2.0f;
    transY = 0.5f + (VIEWPORT_HEIGHT-levelH)/2.0f;
  }*/
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

  //Center the viewport in the window
  viewportMargin.x = (sW-areaWidth)/2.0f;
  viewportMargin.y = (sH-areaHeight)/2.0f;
  glViewport(viewportMargin.x, viewportMargin.y, areaWidth, areaHeight);
  GLW::checkError("glViewport");
  viewportScreenDim.x = areaWidth;
  viewportScreenDim.y = areaHeight;
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

  GLW::ortho(0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT, 0, -1, 1);

  xScreenToGame = VIEWPORT_WIDTH/viewportScreenDim.x;
  yScreenToGame = VIEWPORT_HEIGHT/viewportScreenDim.y;

  if (GameManager::getInstance()->inGame())
    centerGameInViewport();

  glMatrixMode(GL_MODELVIEW);
}

void nativePause () {
  if(!GameManager::created())
    return;

  if (GameManager::getInstance()->getState() == STATE_PLAYING) {
    GameManager::getInstance()->setState(STATE_PAUSED);
  }
}

void nativeRender () {
  static uint64_t lastTime = Utils::getCurrentTimeMillis();

  GameManager::getInstance()->applyTransition();
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();

  //BEGIN time management
  //FIXME: should move this whole timing stuff to timer or similar
  uint64_t now = Utils::getCurrentTimeMillis();

  //FIXME: we don't really need max refresh rate..
  //FIXME: AI could be refreshed much less often
  if (lastTime - now < 50)
    return;

  double elapsedS = (now-lastTime)/1000.0;
  if (Math::epsilonEq(elapsedS, 0))
    return;
  lastTime = now;

  //END time management
  TimerManager::getInstance()->tick(elapsedS);
  inputManager->think(elapsedS);


  if (GameManager::getInstance()->inGame() || GameManager::getInstance()->paused()) {
    if (!GameManager::getInstance()->paused())
      Game::getInstance()->update(elapsedS);

    inputManager->draw();

    glPushMatrix();
    GLW::translate(0.5f, 0.55f, 0);
    gameView->drawLevelIndicator();
    glPopMatrix();

    glPushMatrix();
    GLW::translate(10.0f, 0.55f, 0);
    gameView->drawHearts();
    glPopMatrix();

    glPushMatrix();
    GLW::translate(transX, transY, 0);



    gameView->draw();
    //gameView->debugDraw();
    //gameView->debugDrawAI();
    glPopMatrix();
    
    if (GameManager::getInstance()->inTransition()) {
      const float timeleft = GameManager::getInstance()->getTransitionDelay();
      glPushMatrix();
      GLW::translate(7.5f, 5.0f, 0.0f);
      GLW::scale(15.0f, 10.0f, 0.0f);
      GLW::color(DARK_GREY, 1-timeleft/GameManager::getInstance()->getInitialDelay());
      GLW::disableTextures();
      Square::draw(false);
      GLW::enableTextures();
      GLW::colorWhite();
      glPopMatrix();
    }
    if (GameManager::getInstance()->paused())
      GameManager::getInstance()->drawMenu(); //draw the pause menu
  } else {
    GameManager::getInstance()->drawMenu();
  }
}

void touchEventDown (float x, float y) {
  inputManager->touchEventDown(x,y);
}

void touchEventMove (float x, float y) {
  inputManager->touchEventMove(x,y);
}

void touchEventUp (float x, float y) {
  inputManager->touchEventUp(x,y);
}

void touchEventOther (float x, float y) {
  inputManager->touchEventOther(x,y);
}

