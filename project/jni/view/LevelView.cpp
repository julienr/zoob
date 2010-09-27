#include "LevelView.h"
#include "GLW.h"
#include "view/primitives/Square.h"
#include "GameManager.h"

void LevelView::drawBackground() {
  //Draw a big white quad for the level background
  GLW::disableTextures();
  glPushMatrix();
  GLW::translate(level->getWidth()/2.0 - 0.5, level->getHeight()/2.0 - 0.5, 0);
  //-0.1 because the black sprites for the crates have a slight transparent border around them
  GLW::scale(level->getWidth() - 0.1, level->getHeight() - 0.1, 1);
  Square::draw(false);
  glPopMatrix();
  GLW::enableTextures();
}

void LevelView::drawWalls () {
  //We use a white texture for the walls so we can color them.
  //By default, walls are black
  GLW::color(BLACK);
  for (unsigned x=0; x<level->getWidth(); x++) {
    for (unsigned y=0; y<level->getHeight(); y++) {
      WallEntity* tileEntity = level->getTile(x,y)->getEntity();
      if (tileEntity != NULL) {
        if (tileEntity->isBreakable())
          GLW::color(DARK_GREY);
        wallSprite.draw(tileEntity->getPosition(), tileEntity->getSize());
        if (tileEntity->isBreakable())
          GLW::color(BLACK);
      }
    }
  }
  GLW::color(WHITE);
}
