#include "def.h"
#include "app.h"
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
#include "view/primitives/Square.h"
#include "view/primitives/Circle.h"
#include "net/Server.h"
#include "net/Client.h"
#include "net/ENetServer.h"
#include "net/ENetClient.h"
#include "logic/PlayerCommand.h"


zip* APKArchive;

/*static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}*/

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
#include "net/Server.h"
Level* lvl = NULL;
GameView* gameView = NULL;


#define WON_LOST_DELAY 1000

void startGameCallback (GameManager* manager) {
  manager->setState(STATE_PLAYING);
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

void gameLevelChangedCallback (const char* levelJSON) {
  LevelManager::registerInstance(new SingleLevelManager(levelJSON));
  startGame(0);
}

void toPlayingState () {
  LOGI("[toPlayingState]");
  if (Game::getInstance() && Game::getInstance()->isPaused()) {
    LOGE("unpause");
    Game::getInstance()->unpause();
  } else {
    delete lvl;
    Game::destroy();
    delete gameView;

    lvl = LevelManager::getInstance()->loadLevel(GameManager::getInstance()->getCurrentLevel());
    if (lvl == NULL) {
      LOGE("[toPlayingState] lvl = NULL");
      showMenu(MENU_ERROR, -1);
      GameManager::getInstance()->setState(STATE_NONE);
      return;
    }

    ProgressionManager::getInstance()->changedLevel(lvl);
    if (NetController::isNetworkedGame()) //networked game
      NetworkedGame::create(gameOverCallback, gameWonCallback, lvl);
    else //single player game
      Game::create(gameOverCallback, gameWonCallback, lvl);
    
    gameView = new GameView();

    InputManager::getInstance()->reset();

    centerGameInViewport();
  }
  LOGI("[toPlayingState] finished");
}

void startGame (int level) {
  LOGE("startGame(%i)", level);
  GameManager::getInstance()->setCurrentLevel((size_t)level);
  GameManager::getInstance()->setState(STATE_PLAYING);
}

static void cleanupGame () {
  GameManager::getInstance()->setState(STATE_NONE);
  delete lvl;
  lvl = NULL;
  Game::destroy();
  delete gameView;
  gameView = NULL;
}


void toWonState () {
  LOGE("toWonState");
  int currentLevel = GameManager::getInstance()->getCurrentLevel();
  saveProgress(currentLevel+1);
  const eReward reward = ProgressionManager::getInstance()->getLastReward();
  cleanupGame();
  if (reward != REWARD_NONE) {
    showMenu(reward, currentLevel);
  } else {
    showMenu(MENU_WON, currentLevel);
  }
}

void toLostState () {
  cleanupGame();
  showMenu(MENU_LOST, GameManager::getInstance()->getCurrentLevel());
}

void toEndState () {
  cleanupGame();
  showMenu(MENU_END, GameManager::getInstance()->getCurrentLevel());
}

void toPauseState () {
  Game::getInstance()->pause();
}

bool nativeStartServer () {
  LOGI("Starting server...");
  NetController::registerInstance(new ENetServer());
  if (NetController::getInstance()->start()) {
    startGame(0);
    //TODO: only for debug : server has a different background
    glClearColor(0.4f, 0, 0, 1);
    return true;
  }
  return false;
}

bool nativeStartClient () {
  LOGI("Starting client...");
  NetController::registerInstance(new ENetClient(gameLevelChangedCallback));
  return NetController::getInstance()->start();
}

void nativeInit (const char* apkPath, const char* serie) {
  srand(Utils::getCurrentTimeMillis());
  loadAPK(apkPath);
  nativeLoadSerie(serie);
}

void nativeLoadSerie (const char* serie) {
  LevelManager::registerInstance(new LevelSerieManager(serie));
}

/** Since nativeInitGL will be called on app creation AND each time the opengl 
 * context is recreated, this indicate if we're at app creation (false) or
 * just in context recreation (true) */
bool initialised = false;

void nativeInitGL(int level, int difficulty, int useGamepad, int useTrackball) {
  if (!initialised) {
    initialised = true;
    Difficulty::setDifficulty(difficulty);
    //This is the first time initialisation, we HAVE to instantiate 
    //game manager here because it requires textures
    GameManager::create(startGameCallback, gameUnPauseCallback, level);
    GameManager* gm = GameManager::getInstance();
    gm->setStateCallback(STATE_WON, toWonState);
    gm->setStateCallback(STATE_LOST, toLostState);
    gm->setStateCallback(STATE_END, toEndState);
    gm->setStateCallback(STATE_PLAYING, toPlayingState);
    gm->setStateCallback(STATE_PAUSED, toPauseState);

    InputManager::registerInstance(createInputManager(useGamepad, useTrackball));

    Square::create();
    Circle::create();


    //To avoid noticeable disk access on first explosion, preload them here
    TextureManager::getInstance()->getGroup(TEX_GROUP_GAME)->get("assets/sprites/expl_light.png");
    TextureManager::getInstance()->getGroup(TEX_GROUP_GAME)->get("assets/sprites/expl2.png");

    /*printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);*/
  } else {
    //In case we're coming back from sleep

    //prefs might have changed
    AndroidInputManager* inputManager = static_cast<AndroidInputManager*>(InputManager::getInstance());
    inputManager->setUseGamepad(useGamepad);
    inputManager->setUseTrackball(useTrackball);

    //need to reload textures (GL context might have been destroyed)
    TextureManager::getInstance()->reloadAll();

    if (GameManager::getInstance()->inGame())
      centerGameInViewport();
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
  GameManager::destroy();
  Difficulty::destroy();
  TimerManager::destroy();
  NumberView::destroy();
  ProgressionManager::destroy();
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
 * - the biggest area is the window
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

  //transX = 1.0f;
  transX = InputManager::getInstance()->getLeftXMargin();
  transY = 0.5f + (VIEWPORT_HEIGHT-levelH)/2.0f;
}

void forceRatio (float sW, float sH) {
/**
  * We base all our calculations on a 480/320 = 1.5 aspect ratio (that's the most
  * common resolution for android and iphone)
  * If we have a screen that doesn't have this aspect ratio, we use the biggest square
  * that has this aspect ratio that we can display in this screen and we display
  * in this square.
  * This will of course waste some screen space, but at least the display will look good
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

//Call to switch to STATE_NONE and wait for a new game to be started
void nativeStopGame () {
  cleanupGame();
}

static int debugFlags;

void enableDebug (eDebug what) {
  LOGE("Enable debug for %i", what);
  debugFlags |= (int)what;
}

void disableDebug (eDebug what) {
  debugFlags &= ~(int)what;
}

#ifdef ZOOB_DBG_FPS
static int numFrames = 0;
static int numPhysicFrames = 0;
static uint64_t lastFPS = Utils::getCurrentTimeMillis();
#define FPS_REPORT_INTERVAL 1000 //ms
#endif

//We run physics simulation by chuncks of dt. So the physics framerate is independent
//from the rendering framerate. See Gaffer's "Fix your timestep" article :
// http://gafferongames.com/game-physics/fix-your-timestep/
//We could improve on the current implementation by integration "The final touch" (from the article),
//but this doesn't seem really needed as temporal aliasing is not visible
static const double dt = 1/30.0f; //Physics frequency is 60hz

void nativeRender () {
  static uint64_t lastTime = Utils::getCurrentTimeMillis();

  //Let game manager apply all the transition it has. It normally will have only one, but if during
  //the transition callback, a transition to another state was requested, it might have two or more
  GameManager* gm = GameManager::getInstance();
  do {
    //LOGI("[nativeRender] applyTransition");
    gm->applyTransition();
  } while (gm->inTransition() && gm->getTransitionDelay() <= 0);
  //LOGI("[nativeRender] transitions applied");

  //BEGIN time management
  uint64_t now = Utils::getCurrentTimeMillis();

  double elapsedS = (now-lastTime)/1000.0;

  //DEBUG only : simulate slow
  /*if (elapsedS < 0.1)
    return;*/

  //Only happen if the framerate is REALLY high. Maybe with Moore's law ten years from now :-)
  if (Math::epsilonEq(elapsedS, 0))
    return;

  lastTime = now;

#ifdef ZOOB_DBG_FPS
  if (lastTime - lastFPS > FPS_REPORT_INTERVAL) {
    lastFPS = lastTime;
    const float _secs = FPS_REPORT_INTERVAL/1000.0f;
    LOGE("%i frames (%i physic frames) in %f secondes : %f FPS", numFrames, numPhysicFrames, _secs, numFrames/_secs);
    numFrames = 0;
    numPhysicFrames = 0;
  }
  numFrames++;
#endif

  //END time management
  TimerManager::getInstance()->tick(elapsedS);

  //If we are in a networked game, let the netcontroller think
  if (NetController::isNetworkedGame()) {
    NetController::getInstance()->think(elapsedS);
  }
 
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();

  if (GameManager::getInstance()->getState() == STATE_NONE) {
    return;
  }

  if (GameManager::getInstance()->inGame() || GameManager::getInstance()->paused()) {
    //LOGI("[nativeRender] inGame or paused");
    if (!GameManager::getInstance()->paused()) {
      Game* game = Game::getInstance();
      game->accumulate(elapsedS);
      //LOGI("[nativeRender] accumulator : %lf", game->getAccumulator());
      while (game->getAccumulator() >= dt) { //empty accumulator as much as possible
        PlayerCommand localPlayerCmd;
        //LOGI("[nativeRender] applying commands");
        InputManager::getInstance()->think(elapsedS, localPlayerCmd);
        PlayerTank* playerTank = Game::getInstance()->getPlayerTank();
        if (playerTank)
          Game::getInstance()->applyCommands(playerTank, localPlayerCmd);
        //LOGI("[nativeRender] performing simulation step on game : 0x%x", Game::getInstance());
        Game::getInstance()->update(dt);
        //LOGI("[nativeRender] simulation finished");
        game->useAccumulatedTime(dt);
#ifdef ZOOB_DBG_FPS
        numPhysicFrames++;
#endif
      }
    } else {
      Game::getInstance()->resetAccumulator();
    }
    //LOGI("[nativeRender] rendering");

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

    gameView->draw(elapsedS);
    if (debugFlags & DEBUG_COLLISIONS)
      gameView->debugCollisions();
    if (debugFlags & DEBUG_SHADOWS)
      gameView->debugShadows();
    if (debugFlags & DEBUG_VISIBILITY)
      gameView->debugVisibility();
    if (debugFlags & DEBUG_AI)
      gameView->debugAI();
    if (debugFlags & DEBUG_OVERLAYS)
      gameView->debugOverlays();

    glPopMatrix();
    
    InputManager::getInstance()->draw();

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
  }
}

