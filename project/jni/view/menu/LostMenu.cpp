#include "LostMenu.h"
#include "view/GameManager.h"

LostMenu::LostMenu (GameManager* gm)
  : Menu (gm),
    gameOver("assets/sprites/menuitems/gameover.png"),
    smiley("assets/sprites/menuitems/smiley_unhappy.png"),
    retry("assets/sprites/menuitems/retry.png") {
}

void LostMenu::draw () {
  gameOver.draw(Vector2(7.5,2), Vector2(8,2));
  smiley.draw(Vector2(7.5,5), Vector2(4,4));
  retry.draw(Vector2(7.5, 8), Vector2(8,2));
}

void LostMenu::actionPerformed () {
  gameManager->newGame();
}
