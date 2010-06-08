#include "GameManager.h"
#include "lib/Utils.h"
#include "textures/TextureManager.h"

GameManager* GameManager::instance = NULL;

void GameManager::drawMenu () {
  if (menus[state])
    menus[state]->draw();
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
      TEX_LOCK(TEX_GROUP_MENU);
      break;
    case STATE_TUTORIAL:
      TEX_LOCK(TEX_GROUP_GAME); //tutorial is simply a pause
      TEX_LOCK(TEX_GROUP_TUTORIAL);
      break;
    case STATE_BUY_FULL:
      TEX_LOCK(TEX_GROUP_GET_FULL_VERSION);
      break;
    default:
      ASSERT(false);
  }
  TextureManager::getInstance()->commitLock();
}

