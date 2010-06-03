#include "TutorialMenu.h"
#include "view/GameManager.h"

TutorialMenu::TutorialMenu (GameManager* gm)
  : Menu(gm),
    text("assets/sprites/menuitems/tutorial.png", TEX_GROUP_TUTORIAL)
{}


void TutorialMenu::draw () {
  text.draw(Vector2(7.5, 5), Vector2(10, 10));
}


void TutorialMenu::actionPerformed () {
  gameManager->unpause();
}
