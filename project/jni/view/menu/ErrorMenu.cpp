#include "ErrorMenu.h"
#include "view/GameManager.h"

ErrorMenu::ErrorMenu (GameManager* gm)
  : Menu(gm),
    error("assets/sprites/menuitems/error.png", TEX_GROUP_MENU),
    continueSprite("assets/sprites/menuitems/continue.png", TEX_GROUP_MENU) {
}

ErrorMenu::~ErrorMenu () {

}

void ErrorMenu::draw () {
  error.draw(Vector2(7.5,3), Vector2(8,4));
  continueSprite.draw(Vector2(7.5, 7), Vector2(8,2));
}

void ErrorMenu::actionPerformed () {
  gameManager->goToMenu();
}
