#include "GameManager.h"
#include "lib/Utils.h"

void GameManager::drawMenu () {
  if (menus[state])
    menus[state]->draw();
}

void GameManager::handleTouchDown (const Vector2& p) {
  if (menus[state])
    menus[state]->handleTouchDown(p);
}

void GameManager::handleTouchUp (const Vector2& p) {
  if (menus[state])
    menus[state]->handleTouchUp(p);
}
