#include "PausedMenu.h"
#include "view/GameManager.h"
#include "view/GLW.h"
#include "view/Square.h"

PausedMenu::PausedMenu (GameManager* gm)
  : Menu (gm),
    paused("assets/sprites/menuitems/paused.png", TEX_GROUP_GAME),
    spriteContinue("assets/sprites/menuitems/continue.png", TEX_GROUP_GAME) {
}

void PausedMenu::draw () {
  glPushMatrix();
  GLW::translate(7.5f, 5.0f, 0.0f);
  GLW::scale(15.0f, 10.0f, 0.0f);
  GLW::color(GREY, 0.8f);
  GLW::disableTextures();
  Square::draw(false);
  GLW::enableTextures();
  GLW::colorWhite();
  glPopMatrix();

  paused.draw(Vector2(7.5,3), Vector2(8,2));
  spriteContinue.draw(Vector2(7.5, 5), Vector2(8,2));
}

void PausedMenu::actionPerformed () {
  gameManager->unpause();
}
