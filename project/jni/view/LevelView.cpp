#include "LevelView.h"
#include "GLW.h"
#include "Square.h"
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
  for (unsigned x=0; x<level->getWidth(); x++) {
    for (unsigned y=0; y<level->getHeight(); y++) {
      Entity* tileEntity = level->getTile(x,y)->getEntity();
      if (tileEntity != NULL)
        wallSprite.draw(tileEntity->getPosition(), tileEntity->getSize());
    }
  }
}
