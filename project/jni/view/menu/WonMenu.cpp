#include "WonMenu.h"
#include "view/GameManager.h"

WonMenu::WonMenu (GameManager* gm)
  : Menu (gm),
    gameOver("assets/sprites/menuitems/complete.png"),
    smiley("assets/sprites/menuitems/smiley_happy.png"),
    retry("assets/sprites/menuitems/continue.png") {
}

void WonMenu::draw () {
  gameOver.draw(Vector2(7.5,2), Vector2(8,2));
  smiley.draw(Vector2(7.5,5), Vector2(4,4));
  retry.draw(Vector2(7.5, 8), Vector2(8,2));
}

void WonMenu::actionPerformed () {
  gameManager->nextGame();
}
