#include "WonMenu.h"
#include "view/GameManager.h"

WonMenu::WonMenu (GameManager* gm)
  : Menu (gm),
    complete("assets/sprites/menuitems/complete.png", TEX_GROUP_GAME),
    smiley("assets/sprites/menuitems/smiley_happy.png", TEX_GROUP_GAME),
    nextlvl("assets/sprites/menuitems/continue.png", TEX_GROUP_GAME) {
}

void WonMenu::draw () {
  complete.draw(Vector2(7.5,2), Vector2(8,2));
  smiley.draw(Vector2(7.5,5), Vector2(4,4));
  nextlvl.draw(Vector2(7.5, 8), Vector2(8,2));
}

void WonMenu::actionPerformed () {
  gameManager->nextGame();
}
