#include "ControlOptionMenu.h"
#include "view/GameManager.h"
#include "input/AndroidInputManager.h"

#define MENU_ITEM_BACK 0
#define MENU_ITEM_CONTROL_NEXT 1
#define MENU_ITEM_CONTROL_PREV 2

ControlOptionMenu::ControlOptionMenu(GameManager* gm)
  : Menu(gm),
    inputTouch("assets/sprites/menuitems/input_touch.png", TEX_GROUP_MENU),
    inputTrackball("assets/sprites/menuitems/input_trackball.png", TEX_GROUP_MENU),
    optionDesc("assets/sprites/menuitems/controls_option.png", TEX_GROUP_MENU) {
  addItem(new MenuItem("assets/sprites/menuitems/back.png",
                       "assets/sprites/menuitems/back_h.png",
                       MENU_ITEM_BACK,
                       TEX_GROUP_MENU));

  addItem(new MenuItem("assets/sprites/menuitems/next.png",
                       "assets/sprites/menuitems/next_h.png",
                       MENU_ITEM_CONTROL_NEXT,
                       TEX_GROUP_MENU));

  addItem(new MenuItem("assets/sprites/menuitems/prev.png",
                       "assets/sprites/menuitems/prev.png",
                       MENU_ITEM_CONTROL_PREV,
                       TEX_GROUP_MENU));
  _initItems();
}

ControlOptionMenu::~ControlOptionMenu() {
}

Vector2 inputDescPos(8,4);

AndroidInputManager* getInputManager () {
  //FIXME: UGLY ! NO WAY TO ENSURE THIS IS AN ANDROID_INPUT_MANAGER
  AndroidInputManager* inputManager = static_cast<AndroidInputManager*>(InputManager::getInstance());
  return inputManager;
}

void ControlOptionMenu::draw () {
  eInputMode mode = getInputManager()->getInputMode();
  switch (mode) {
    case INPUT_TOUCH:
      inputTouch.draw(inputDescPos, Vector2(8,4));
      break;
    case INPUT_TRACKBALL:
      inputTrackball.draw(inputDescPos, Vector2(8,4));
      break;
  }

  optionDesc.draw(Vector2(4, 1), Vector2(8, 2));

  for (size_t i=0; i<numItems(); i++) {
     MenuItem* item = getItem(i);
     short id = item->getID();
     if ((id == MENU_ITEM_CONTROL_PREV && mode == INPUT_TOUCH) ||
         (id == MENU_ITEM_CONTROL_NEXT && mode == INPUT_TRACKBALL))
       continue;

     item->draw();
  }
}

void ControlOptionMenu::actionPerformed (short touchedItem) {
  switch (touchedItem) {
    case MENU_ITEM_BACK: _actionBack(); break;
    case MENU_ITEM_CONTROL_NEXT: _actionNextCtrl(); break;
    case MENU_ITEM_CONTROL_PREV: _actionPrevCtrl(); break;
    default:
      LOGE("unhandled menu id : %i", touchedItem);
      ASSERT(false);
      break;
  }
}

void ControlOptionMenu::_actionBack() {
  gameManager->setState(STATE_MAINMENU);
}

void ControlOptionMenu::_actionNextCtrl () {
  getInputManager()->setInputMode(INPUT_TRACKBALL);
}

void ControlOptionMenu::_actionPrevCtrl () {
  getInputManager()->setInputMode(INPUT_TOUCH);
}

void ControlOptionMenu::_initItems () {
  for (size_t i=0; i<numItems(); i++) {
     MenuItem* mi = getItem(i);
     switch (mi->getID()) {
       case MENU_ITEM_BACK:
         mi->setPosition(Vector2(12, 8));
         mi->setSize(Vector2(4, 2));
         mi->setBB(mi->getPosition(), Vector2(4, 2));
         break;
       case MENU_ITEM_CONTROL_NEXT:
         mi->setPosition(Vector2(12,4));
         mi->setSize(Vector2(2,2));
         mi->setBB(mi->getPosition(), Vector2(2.5,2));
         break;
       case MENU_ITEM_CONTROL_PREV:
         mi->setPosition(Vector2(2.5, 4));
         mi->setSize(Vector2(2,2));
         mi->setBB(mi->getPosition(), Vector2(2.5,2));
         break;
       default:
         LOGE("unhandled menu id : %i", mi->getID());
         ASSERT(false);
         break;
     }
  }
}
