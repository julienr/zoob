#include "MainMenu.h"
#include "view/GameManager.h"
#include "view/NumberView.h"

#define MENU_ITEM_START 0
#define MENU_ITEM_LVL_NEXT 1
#define MENU_ITEM_LVL_PREV 2
#define MENU_ITEM_DIFF_NEXT 3
#define MENU_ITEM_DIFF_PREV 4

MainMenu::MainMenu (GameManager* gm)
  : Menu (gm),
    logo("assets/sprites/logo.png", TEX_GROUP_MENU),
    easy("assets/sprites/menuitems/easy.png", TEX_GROUP_MENU),
    normal("assets/sprites/menuitems/normal.png", TEX_GROUP_MENU),
#if !FULL_VERSION
    demo("assets/sprites/demo.png", TEX_GROUP_MENU),
#endif
    hard("assets/sprites/menuitems/hard.png", TEX_GROUP_MENU)
   {
   addItem(new MenuItem("assets/sprites/menuitems/start.png",
                              "assets/sprites/menuitems/start_h.png",
                              MENU_ITEM_START,
                              TEX_GROUP_MENU));
   addItem(new MenuItem("assets/sprites/menuitems/next.png",
                               "assets/sprites/menuitems/next_h.png",
                               MENU_ITEM_LVL_NEXT,
                               TEX_GROUP_MENU,
                               true));
   addItem(new MenuItem("assets/sprites/menuitems/prev.png",
                               "assets/sprites/menuitems/prev_h.png",
                               MENU_ITEM_LVL_PREV,
                               TEX_GROUP_MENU,
                               true));
   addItem(new MenuItem("assets/sprites/menuitems/next.png",
                               "assets/sprites/menuitems/next_h.png",
                               MENU_ITEM_DIFF_NEXT,
                               TEX_GROUP_MENU));
   addItem(new MenuItem("assets/sprites/menuitems/prev.png",
                               "assets/sprites/menuitems/prev_h.png",
                               MENU_ITEM_DIFF_PREV,
                               TEX_GROUP_MENU));
   _initItems();
}

MainMenu::~MainMenu () {
}

void MainMenu::draw () {
  logo.draw(logoPos, logoSize);
#if !FULL_VERSION
  demo.draw(logoPos+Vector2(1.5,3), Vector2(4, 2));
#endif
  const int currentDiff = Difficulty::getInstance()->currentDifficulty();
  for (size_t i=0; i<numItems(); i++) {
    MenuItem* item = getItem(i);
    short id = item->getID();
    if ((id == MENU_ITEM_LVL_NEXT && gameManager->isAtLastLevel()) ||
        (id == MENU_ITEM_LVL_PREV && gameManager->isAtFirstLevel()))
      continue;
    if ((id == MENU_ITEM_DIFF_NEXT && currentDiff == MAX_DIFFICULTY) ||
        (id == MENU_ITEM_DIFF_PREV && currentDiff == 0))
      continue;
    item->draw();
  }
  NumberView::getInstance()->drawInt(gameManager->getCurrentLevel(), numberPos, numberSize);
  switch (currentDiff) {
    case 0: easy.draw(diffPos, diffSize); break;
    case 1: normal.draw(diffPos, diffSize); break;
    case 2: hard.draw(diffPos, diffSize); break;
    default: ASSERT(false);
  }
  //font->print("1234", Vector2(1,3), 1.5);
}

void MainMenu::actionPerformed (short touchedItem) {
  switch (touchedItem) {
    case MENU_ITEM_START: _actionStart(); break;
    case MENU_ITEM_LVL_NEXT: _actionNextLvl(); break;
    case MENU_ITEM_LVL_PREV: _actionPrevLvl(); break;
    case MENU_ITEM_DIFF_NEXT: _actionNextDiff(); break;
    case MENU_ITEM_DIFF_PREV: _actionPrevDiff(); break;
    default:
      LOGE("unhandled menu id : %i", touchedItem);
      ASSERT(false);
      break;
  }
}

void MainMenu::_actionStart () {
  gameManager->newGame();
}

void MainMenu::_actionNextLvl () {
  gameManager->nextLevel();
}

void MainMenu::_actionPrevLvl () {
  gameManager->prevLevel();
}

void MainMenu::_actionNextDiff () {
  const int currentDiff = Difficulty::getInstance()->currentDifficulty();
  if (currentDiff == MAX_DIFFICULTY)
    return;
  Difficulty::getInstance()->setDifficulty(currentDiff+1);
}

void MainMenu::_actionPrevDiff () {
  const int currentDiff = Difficulty::getInstance()->currentDifficulty();
  if (currentDiff == 0)
    return;
  Difficulty::getInstance()->setDifficulty(currentDiff-1);
}

void MainMenu::_initItems () {
  logoPos = Vector2(3,5);
  logoSize = Vector2(8,8);

  numberPos = Vector2(9, 2);
  numberSize = Vector2(3,3);

  diffPos = Vector2(9.5, 7.2f);
  diffSize = Vector2(8, 4);

  for (size_t i=0; i<numItems(); i++) {
    MenuItem* mi = getItem(i);
    switch (mi->getID()) {
      case MENU_ITEM_START:
        mi->setPosition(Vector2(11, 4.5f));
        mi->setSize(Vector2(8, 4));
        mi->setBB(mi->getPosition(), Vector2(8,2));
        break;
      case MENU_ITEM_LVL_NEXT:
        mi->setPosition(Vector2(11,2));
        mi->setSize(Vector2(2,2));
        mi->setBB(mi->getPosition(), Vector2(2.5,2.5));
        break;
      case MENU_ITEM_LVL_PREV:
        mi->setPosition(Vector2(7, 2));
        mi->setSize(Vector2(2,2));
        mi->setBB(mi->getPosition(), Vector2(2.5,2.5));
        break;
      case MENU_ITEM_DIFF_PREV:
        mi->setPosition(Vector2(5.5, 7.2f));
        mi->setSize(Vector2(2,2));
        mi->setBB(mi->getPosition(), Vector2(2.5,2.5));
        break;
      case MENU_ITEM_DIFF_NEXT:
        mi->setPosition(Vector2(14, 7.2f));
        mi->setSize(Vector2(2,2));
        mi->setBB(mi->getPosition(), Vector2(2.5,2.5));
        break;
      default:
        LOGE("unhandled menu id : %i", mi->getID());
        ASSERT(false);
        break;
    }
  }
}
