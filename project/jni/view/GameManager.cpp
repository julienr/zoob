#include "GameManager.h"
#include "lib/Utils.h"
#include "textures/TextureManager.h"
#include "view/menu/RewardMenu.h"
#include "view/menu/ControlOptionMenu.h"

GameManager* GameManager::instance = NULL;

GameManager::GameManager (startGameCallback_t gameCb,
             callback_t menuCb,
             callback_t continueCb,
             int levelLimit)
  : newGameCB(gameCb),
    menuCB(menuCb),
    continueCB(continueCb),
    currentLevel(levelLimit),
    levelLimit(levelLimit),
    stateTransition(-1),
    transitionDelay(0) {
  menus[STATE_PLAYING] = NULL;
  menus[STATE_MAINMENU] = new MainMenu(this);
  menus[STATE_LOST] = new LostMenu(this);
  menus[STATE_WON] = new WonMenu(this);
  menus[STATE_END] = new EndMenu(this);
  menus[STATE_PAUSED] = new PausedMenu(this);
  menus[STATE_TUTORIAL] = new TutorialMenu(this);
  menus[STATE_BUY_FULL] = new BuyFullMenu(this);
  menus[STATE_REWARD] = new RewardMenu(this);
  menus[STATE_CONTROL_OPTIONS] = new ControlOptionMenu(this);

  for (int i=0; i<MAX_STATE; i++)
    stateCallbacks[i] = NULL;
#if FULL_VERSION
  state = STATE_MAINMENU;
#else
  state = STATE_BUY_FULL;
#endif
  applyLocks();
}


void GameManager::drawMenu () {
  if (menus[state]) {
    menus[state]->think();
    menus[state]->draw();
  }
}

void GameManager::handleTouchDown (const Vector2& p) {
  if (menus[state])
    menus[state]->handleTouchDown(p);
  stateAtTouchDown = state;
}

void GameManager::handleTouchUp (const Vector2& p) {
  if (stateAtTouchDown == state && menus[state])
    menus[state]->handleTouchUp(p);
}

void GameManager::applyTransition () {
  static uint64_t last = Utils::getCurrentTimeMillis();
  const uint64_t now = Utils::getCurrentTimeMillis();
  if (stateTransition != -1) {
    transitionDelay -= (int)(now-last);
    if (transitionDelay <= 0) {
      state = (eAppState)stateTransition;
      applyLocks();
      stateTransition = -1;
      transitionDelay = 0;
      if (stateCallbacks[state] != NULL)
        stateCallbacks[state]();
    }
  }
  last = now;
}

#define TEX_LOCK(i) TextureManager::getInstance()->lockGroup(i)

void GameManager::applyLocks () {
  TextureManager::getInstance()->startLock();
  TEX_LOCK(TEX_GROUP_UTILS);
  switch (state) {
    case STATE_PAUSED:
    case STATE_PLAYING:
    case STATE_LOST:
    case STATE_WON:
    case STATE_END:
      TEX_LOCK(TEX_GROUP_GAME);
      break;
    case STATE_MAINMENU:
    case STATE_CONTROL_OPTIONS:
      TEX_LOCK(TEX_GROUP_MENU);
      break;
    case STATE_TUTORIAL:
      TEX_LOCK(TEX_GROUP_GAME); //tutorial is simply a pause
      TEX_LOCK(TEX_GROUP_TUTORIAL);
      break;
    case STATE_BUY_FULL:
      TEX_LOCK(TEX_GROUP_GET_FULL_VERSION);
      break;
    case STATE_REWARD:
      TEX_LOCK(TEX_GROUP_REWARD);
      break;
    default:
      ASSERT(false);
  }
  TextureManager::getInstance()->commitLock();
}

