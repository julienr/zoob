#include "LevelView.h"
#include "GLW.h"
#include "Square.h"

void LevelView::draw () {
  //Draw a big white quad for the level background
  GLW::disableTextures();
  glPushMatrix();
  GLW::translate(level->getWidth()/2.0 - 0.5, level->getHeight()/2.0 - 0.5, 0);
  //-0.1 because the black sprites for the crates have a slight transparent border around them
  GLW::scale(level->getWidth() - 0.1, level->getHeight() - 0.1, 1);
  Square::draw(false);
  glPopMatrix();
  GLW::enableTextures();

  for (unsigned x=0; x<level->getWidth(); x++) {
    for (unsigned y=0; y<level->getHeight(); y++) {
      switch (level->getTile(x,y)->getType()) {
        case E: /*emptySprite.draw(Vector2(x,y), Vector2(1,1));*/ break;
        case S: /*spawnSprite.draw(Vector2(x,y), Vector2(1,1));*/ break;
        case W:
        case R:
        case B:
        case L:
        case T: {
          Entity* tileEntity = level->getTile(x,y)->getEntity();
          ASSERT(tileEntity != NULL);
          wallSprite.draw(tileEntity->getPosition(), tileEntity->getSize());
          break;
        }
      }
    }
  }
}
