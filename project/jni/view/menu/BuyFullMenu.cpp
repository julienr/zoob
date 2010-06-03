#include "BuyFullMenu.h"
#include "MenuItem.h"
#include "view/GameManager.h"

#define MENU_ITEM_YES 0
#define MENU_ITEM_NO 1

BuyFullMenu::BuyFullMenu (GameManager* gm)
  : Menu(gm),
    text("assets/sprites/menuitems/get_full.png", TEX_GROUP_GET_FULL_VERSION)
{
  MenuItem* yesItem = new MenuItem("assets/sprites/menuitems/get_full_yes.png",
                       "assets/sprites/menuitems/get_full_yes_h.png",
                       MENU_ITEM_YES,
                       TEX_GROUP_GET_FULL_VERSION);
  yesItem->setPosition(Vector2(5, 7));
  yesItem->setSize(Vector2(4, 4));
  yesItem->setBB(yesItem->getPosition(), Vector2(4,4));

  MenuItem* noItem = new MenuItem("assets/sprites/menuitems/get_full_no.png",
                       "assets/sprites/menuitems/get_full_no_h.png",
                       MENU_ITEM_NO,
                       TEX_GROUP_GET_FULL_VERSION);
  noItem->setPosition(Vector2(10, 7));
  noItem->setSize(Vector2(4, 2));
  noItem->setBB(noItem->getPosition(), Vector2(4, 2));

  addItem(yesItem);
  addItem(noItem);
}

void BuyFullMenu::draw () {
  text.draw(Vector2(7.5, 3), Vector2(10, 5));

  for (size_t i=0; i<numItems(); i++) {
    getItem(i)->draw();
  }
}

void BuyFullMenu::actionPerformed(short touchedItem) {
  switch (touchedItem) {
    case MENU_ITEM_YES: _actionYes(); break;
    case MENU_ITEM_NO: _actionNo(); break;
    default:
      LOGE("unhandled menu id : %i", touchedItem);
      ASSERT(false);
      break;
  }
}

void BuyFullMenu::_actionYes () {
  LOGE("YES");
}

void BuyFullMenu::_actionNo () {
  GameManager::getInstance()->setState(STATE_MAINMENU);
}
