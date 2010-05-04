#include "Menu.h"

Menu::~Menu () {
  for (size_t i=0; i<menuItems.length(); i++)
    delete menuItems[i];
}

bool Menu::inside (const Vector2& p) {
  for (size_t i=0; i<menuItems.length(); i++)
    if (menuItems[i]->inside(p))
      return true;
  return false;
}

void Menu::handleTouchDown (const Vector2& p) {
  touchedItem = -1;
  for (size_t i=0; i<menuItems.length(); i++) {
    if (menuItems[i]->inside(p)) {
      menuItems[i]->setPressed(true);
      touchedItem = menuItems[i]->getID();
      return;
    }
  }
}

void Menu::handleTouchUp (const Vector2& p) {
  //WARNING: this depends on the order of adding to menuItems in GameManager constructor
  if (touchedItem != -1) {
    const size_t i = (size_t)touchedItem;
    menuItems[i]->setPressed(false);
    if (menuItems[i]->inside(p)) //only validate the action if we're still inside on touchup
      actionPerformed(touchedItem);
  } else
    actionPerformed();
  touchedItem = -1;
}
