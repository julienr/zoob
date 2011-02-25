#include "GameManager.h"
#include "lib/Utils.h"
#include "textures/TextureManager.h"

GameManager* GameManager::instance = NULL;

GameManager::GameManager (callback_t gameCb,
                          callback_t continueCb,
                          int levelLimit)
  : newGameCB(gameCb),
    continueCB(continueCb),
    currentLevel(levelLimit),
    levelLimit(levelLimit),
    stateTransition(-1),
    transitionDelay(0) {
  menus[STATE_PLAYING] = NULL;
  menus[STATE_PAUSED] = new PausedMenu(this);

  for (int i=0; i<MAX_STATE; i++)
    stateCallbacks[i] = NULL;
  state = STATE_NONE;
/*#if FULL_VERSION
  state = STATE_MAINMENU;
#else
  state = STATE_BUY_FULL;
#endif*/
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
  if (state == STATE_NONE)
    return;
  TextureManager::getInstance()->startLock();
  TEX_LOCK(TEX_GROUP_UTILS);
  switch (state) {
    case STATE_WON:
    case STATE_END:
    case STATE_LOST:
    case STATE_PAUSED:
    case STATE_PLAYING:
      TEX_LOCK(TEX_GROUP_GAME);
      break;
    default:
      ASSERT(false);
  }
  TextureManager::getInstance()->commitLock();
}

