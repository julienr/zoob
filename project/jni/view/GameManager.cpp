#include "GameManager.h"
#include "lib/Utils.h"

void GameManager::drawMenu () {
  mainMenu.draw();
}

void GameManager::resize (float sW, float sH) {
  screenWidth = sW;
  screenHeight = sH;

  mainMenu.resize(sW, sH);
}

void GameManager::handleTouchDown (const Vector2& p) {
  mainMenu.handleTouchDown(p);
}

void GameManager::handleTouchUp (const Vector2& p) {
  mainMenu.handleTouchUp(p);
}
