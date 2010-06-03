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

#define TEX_LOCK(i) TextureManager::getInstance()->lockGroup(i)

void applyLocks (eAppState s) {
  TextureManager::getInstance()->startLock();
  TEX_LOCK(TEX_GROUP_UTILS);
  switch (s) {
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

void GameManager::setState (eAppState s) {
  state = s;
  applyLocks(state);
}
