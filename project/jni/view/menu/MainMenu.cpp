#include "MainMenu.h"
#include "view/GameManager.h"

#define MENU_ITEM_START 0
#define MENU_ITEM_OPTIONS 1
#define MENU_ITEM_NEXT 2
#define MENU_ITEM_PREV 3

MainMenu::MainMenu (GameManager* gm)
  : Menu (gm),
    logo("assets/sprites/logo.png") {
   addItem(new MenuItem("assets/sprites/menuitems/start.png",
                              "assets/sprites/menuitems/start_h.png",
                              MENU_ITEM_START));
   addItem(new MenuItem("assets/sprites/menuitems/options.png",
                               "assets/sprites/menuitems/options_h.png",
                               MENU_ITEM_OPTIONS));
   addItem(new MenuItem("assets/sprites/menuitems/next.png",
                               "assets/sprites/menuitems/next_h.png",
                               MENU_ITEM_NEXT));
   addItem(new MenuItem("assets/sprites/menuitems/prev.png",
                               "assets/sprites/menuitems/prev_h.png",
                               MENU_ITEM_PREV));

   numbers[0] = new Sprite("assets/sprites/menuitems/0.png");
   numbers[1] = new Sprite("assets/sprites/menuitems/1.png");
   numbers[2] = new Sprite("assets/sprites/menuitems/2.png");
   numbers[3] = new Sprite("assets/sprites/menuitems/3.png");
}

MainMenu::~MainMenu () {
  for (int i=0; i<3; i++)
    delete numbers[i];
}

void MainMenu::draw () {
  logo.draw(logoPos, logoSize);
  for (size_t i=0; i<numItems(); i++) {
    MenuItem* item = getItem(i);
    short id = item->getID();
    if ((id == MENU_ITEM_NEXT && gameManager->isAtLastLevel()) ||
        (id == MENU_ITEM_PREV && gameManager->isAtFirstLevel()))
      continue;
    if (getTouchedItem() != -1 && getTouchedItem() == id)
      item->drawHover();
    else
      item->draw();
  }
  numbers[gameManager->getCurrentLevel()]->draw(numberPos, numberSize);
  //font->print("1234", Vector2(1,3), 1.5);
}

void MainMenu::actionPerformed (short touchedItem) {
  switch (touchedItem) {
    case MENU_ITEM_START: _actionStart(); break;
    case MENU_ITEM_OPTIONS: _actionOptions(); break;
    case MENU_ITEM_NEXT: _actionNext(); break;
    case MENU_ITEM_PREV: _actionPrev(); break;
    default:
      LOGE("unhandled menu id : %i", touchedItem);
      ASSERT(false);
      break;
  }
}

void MainMenu::_actionStart () {
  gameManager->newGame();
}

void MainMenu::_actionOptions () {
  LOGE("Options");
}

void MainMenu::_actionNext () {
  gameManager->nextLevel();
}

void MainMenu::_actionPrev () {
  gameManager->prevLevel();
}

void MainMenu::resize (float sW, float sH) {
  //FIXME: all the sizes/positions should be adapted depending on sW/sH

  logoPos = Vector2(3,4);
  logoSize = Vector2(8,8);

  numberPos = Vector2(8.9, 2);
  numberSize = Vector2(3.5,3.5);

  for (size_t i=0; i<numItems(); i++) {
    MenuItem* mi = getItem(i);
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
