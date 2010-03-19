#include "LevelView.h"

void LevelView::draw () {
  for (unsigned x=0; x<level->getWidth(); x++) {
    for (unsigned y=0; y<level->getHeight(); y++) {
      const Tile* tile = level->getTile(x,y);
      switch (tile->getContentType()) {
        case E: emptySprite.draw(*tile); break;
        case S: spawnSprite.draw(*tile); break;
        case W: wallSprite.draw(*tile); break;
      }
    }
  }
}
