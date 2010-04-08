#include "GameManager.h"
#include "lib/Utils.h"

void GameManager::drawMenu () {
  for (size_t i=0; i<menuItems.length(); i++) {
    if (touchedItem != -1 && (size_t)touchedItem == i)
      menuItems[i]->drawHover();
    else
      menuItems[i]->draw();
  }
}


void GameManager::handleTouchDown (const Vector2& p) {
  for (size_t i=0; i<menuItems.length(); i++)
    if (menuItems[i]->inside(p))
      touchedItem = i;
}

void GameManager::handleTouchUp () {
  //WARNING: this depends on the order of adding to menuItems in GameManager constructor
  switch (touchedItem) {
    case 0: _actionRetry(); break;
    case 1: _actionNextLvl(); break;
    default: break;
  }
  touchedItem = -1;
}

void GameManager::_actionRetry () {
  state = STATE_PLAYING;
  newGameCB(this);
}

void GameManager::_actionNextLvl () {
  state = STATE_PLAYING;
  currentLevel++;
  newGameCB(this);
}

void GameManager::resize (float sW, float sH) {
  screenWidth = sW;
  screenHeight = sH;

  const float size = 4.0f;
  for (size_t i=0; i<menuItems.length(); i++) {
    menuItems[i]->setPosition(Vector2(screenWidth/2.0f, 2.0f+i*size));
    menuItems[i]->setSize(Vector2(size, size));
  }
}
