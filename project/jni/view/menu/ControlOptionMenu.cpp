#include "ControlOptionMenu.h"
#include "view/GameManager.h"
#include "input/AndroidInputManager.h"

#define MENU_ITEM_BACK 0
#define MENU_ITEM_CONTROL_NEXT 1
#define MENU_ITEM_CONTROL_PREV 2
#define MENU_ITEM_TRACKBALL_CHECKBOX 3

AndroidInputManager* getInputManager () {
  //FIXME: UGLY ! NO WAY TO ENSURE THIS IS AN ANDROID_INPUT_MANAGER
  AndroidInputManager* inputManager = static_cast<AndroidInputManager*>(InputManager::getInstance());
  return inputManager;
}

ControlOptionMenu::ControlOptionMenu(GameManager* gm)
  : Menu(gm),
    inputTouch("assets/sprites/menuitems/input_touch.png", TEX_GROUP_MENU),
    inputGamepad("assets/sprites/menuitems/input_gamepad.png", TEX_GROUP_MENU),
    optionDesc("assets/sprites/menuitems/controls_option.png", TEX_GROUP_MENU),
    trackballDesc("assets/sprites/menuitems/trackball.png", TEX_GROUP_MENU),
    star("assets/sprites/menuitems/star.png", TEX_GROUP_MENU){
  addItem(new Button("assets/sprites/menuitems/back.png",
                       "assets/sprites/menuitems/back_h.png",
                       MENU_ITEM_BACK,
                       TEX_GROUP_MENU));

  addItem(new Button("assets/sprites/menuitems/next.png",
                       "assets/sprites/menuitems/next_h.png",
                       MENU_ITEM_CONTROL_NEXT,
                       TEX_GROUP_MENU));

  addItem(new Button("assets/sprites/menuitems/prev.png",
                       "assets/sprites/menuitems/prev_h.png",
                       MENU_ITEM_CONTROL_PREV,
                       TEX_GROUP_MENU));

  trackballCheckbox = new Checkbox(MENU_ITEM_TRACKBALL_CHECKBOX,
                                  TEX_GROUP_MENU,
                                  false);
  addItem(trackballCheckbox);
  _initItems();
}

ControlOptionMenu::~ControlOptionMenu() {
}

Vector2 inputDescPos(8,3.5);
Vector2 inputTrackballPos(6.5, 7);

void ControlOptionMenu::draw () {
  eInputMode mode = getInputManager()->getInputMode();
  switch (mode) {
    case INPUT_TOUCH:
      inputTouch.draw(inputDescPos, Vector2(8,4));
      break;
    case INPUT_GAMEPAD:
      inputGamepad.draw(inputDescPos, Vector2(8,4));
      break;
    default:
      ASSERT(false);
  }

  trackballCheckbox->setChecked(getInputManager()->getUseTrackball());

  star.draw(Vector2(1,1), Vector2(1,1), M_PI/3);
  star.draw(Vector2(2, 2), Vector2(1.5, 1.5));
  star.draw(Vector2(6, 1), Vector2(2, 2), M_PI/6);
  optionDesc.draw(Vector2(5, 1), Vector2(8, 2));
  trackballDesc.draw(inputTrackballPos, Vector2(8, 2));

  for (size_t i=0; i<numItems(); i++) {
     MenuItem* item = getItem(i);
     short id = item->getID();
     if ((id == MENU_ITEM_CONTROL_PREV && mode == 0) ||
         (id == MENU_ITEM_CONTROL_NEXT && mode == NUM_INPUT_METHODS-1))
       continue;

     item->draw();
  }
}

void ControlOptionMenu::actionPerformed (short touchedItem) {
  switch (touchedItem) {
    case MENU_ITEM_BACK: _actionBack(); break;
    case MENU_ITEM_CONTROL_NEXT: _actionNextCtrl(); break;
    case MENU_ITEM_CONTROL_PREV: _actionPrevCtrl(); break;
    case MENU_ITEM_TRACKBALL_CHECKBOX: _actionTrackballToggle(); break;
    default:
      LOGE("unhandled menu id : %i", touchedItem);
      ASSERT(false);
      break;
  }
}

void ControlOptionMenu::_actionTrackballToggle () {
  trackballCheckbox->toggle();
  getInputManager()->setUseTrackball(trackballCheckbox->isChecked());
}

void ControlOptionMenu::_actionBack() {
  gameManager->setState(STATE_MAINMENU);
}

void ControlOptionMenu::_actionNextCtrl () {
  eInputMode mode = getInputManager()->getInputMode();
  if (mode == NUM_INPUT_METHODS-1)
    return;
  getInputManager()->setInputMode((eInputMode)(mode+1));
}

void ControlOptionMenu::_actionPrevCtrl () {
  eInputMode mode = getInputManager()->getInputMode();
    if (mode == 0)
      return;
  getInputManager()->setInputMode((eInputMode)(mode-1));
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
         mi->setPosition(Vector2(12,inputDescPos.y));
         mi->setSize(Vector2(2,2));
         mi->setBB(mi->getPosition(), Vector2(2.5,2));
         break;
       case MENU_ITEM_CONTROL_PREV:
         mi->setPosition(Vector2(2.5,inputDescPos.y));
         mi->setSize(Vector2(2,2));
         mi->setBB(mi->getPosition(), Vector2(2.5,2));
         break;
       case MENU_ITEM_TRACKBALL_CHECKBOX:
         mi->setPosition(Vector2(2.5, inputTrackballPos.y));
         mi->setSize(Vector2(1.5,1.5));
         mi->setBB(mi->getPosition(), Vector2(2,2));
         break;
       default:
         LOGE("unhandled menu id : %i", mi->getID());
         ASSERT(false);
         break;
     }
  }
}
