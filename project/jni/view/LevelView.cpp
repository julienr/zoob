#include "LevelView.h"

void LevelView::draw () {
  for (unsigned x=0; x<level->getWidth(); x++) {
    for (unsigned y=0; y<level->getHeight(); y++) {
      switch (level->getTile(x,y)->getType()) {
        case E: emptySprite.draw(Vector2(x,y), Vector2(1,1)); break;
        case S: spawnSprite.draw(Vector2(x,y), Vector2(1,1)); break;
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
