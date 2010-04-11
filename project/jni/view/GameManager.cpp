#include "GameManager.h"
#include "lib/Utils.h"

void GameManager::drawMenu () {
  mainMenu.draw();
}

void GameManager::handleTouchDown (const Vector2& p) {
  mainMenu.handleTouchDown(p);
}

void GameManager::handleTouchUp (const Vector2& p) {
  mainMenu.handleTouchUp(p);
}
