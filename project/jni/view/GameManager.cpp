#include "GameManager.h"
#include "lib/Utils.h"
#include "levels/LevelsData.h"

void GameManager::drawMenu () {
  logo.draw(logoPos, logoSize);
  for (size_t i=0; i<menuItems.length(); i++) {
    if (touchedItem != -1 && touchedItem == menuItems[i]->getID())
      menuItems[i]->drawHover();
    else
      menuItems[i]->draw();
  }
  numbers[currentLevel]->draw(numberPos, numberSize);
  //font->print("1234", Vector2(1,3), 1.5);
}


void GameManager::handleTouchDown (const Vector2& p) {
  touchedItem = -1;
  for (size_t i=0; i<menuItems.length(); i++)
    if (menuItems[i]->inside(p))
      touchedItem = menuItems[i]->getID();
}

void GameManager::handleTouchUp (const Vector2& p) {
  //WARNING: this depends on the order of adding to menuItems in GameManager constructor
  if (touchedItem != -1 && menuItems[(size_t)touchedItem]->inside(p)) {
    switch (touchedItem) {
      case 0: _actionRetry(); break;
      case 1: _actionNextLvl(); break;
      default: break;
    }
  }
  touchedItem = -1;
}

void GameManager::_actionRetry () {
  //state = STATE_PLAYING;
  //newGameCB(this);
}

void GameManager::_actionNextLvl () {
  //state = STATE_PLAYING;
  //currentLevel = (currentLevel+1)%numLevels;
  //newGameCB(this);
}

void GameManager::resize (float sW, float sH) {
  screenWidth = sW;
  screenHeight = sH;

  logoPos = Vector2(3,4);
  logoSize = Vector2(8,8);

  numberPos = Vector2(8.9, 2);
  numberSize = Vector2(3.5,3.5);

  const float size = 2.0f;
  for (size_t i=0; i<menuItems.length(); i++) {
    MenuItem* mi = menuItems[i];
    switch (mi->getID()) {
      case MENU_ITEM_START:
        mi->setPosition(Vector2(11, 4.5f));
        mi->setSize(Vector2(8, 4));
        mi->setBB(Vector2(11,4.5f), Vector2(8,2));
        break;
      case MENU_ITEM_OPTIONS:
        mi->setPosition(Vector2(11, 7.2f));
        mi->setSize(Vector2(8, 4));
        mi->setBB(Vector2(11,7.2f), Vector2(8,2.5));
        break;
      case MENU_ITEM_NEXT:
        mi->setPosition(Vector2(11,2));
        mi->setSize(Vector2(2,2));
        mi->setBB(Vector2(11,2), Vector2(2,2));
        break;
      case MENU_ITEM_PREV:
        mi->setPosition(Vector2(7, 2));
        mi->setSize(Vector2(2,2));
        mi->setBB(Vector2(7,2), Vector2(2,2));
        break;
      default:
        LOGE("unhandled menu id : %i", mi->getID());
        ASSERT(false);
        break;
    }
  }
}
