#include "Level.h"
#include "Trigger.h"

Tile::Tile(int x, int y, eTileType t): type(t) {
  switch(t) {
    case W: entity = new WallEntity(1, 1, Vector2(x,y)); break;
    case E: entity = NULL; break;
    case T: entity = new WallEntity(1, 0.5f, Vector2(x,y-0.25f)); break;
    case L: entity = new WallEntity(0.5f, 1, Vector2(x-0.25f,y)); break;
    case B: entity = new WallEntity(1, 0.5f, Vector2(x,y+0.25f)); break;
    //FIXME: have to use 0.24f instead of 0.25f to avoid rendering glitches...
    case R: entity = new WallEntity(0.5f, 1, Vector2(x+0.24f,y)); break;
    case M: entity = new WallEntity(0.5f, 0.5f, Vector2(x,y)); break;
    case TL: entity = new WallEntity(0.5f, 0.5f, Vector2(x-0.25f,y-0.25f)); break;
    case TR: entity = new WallEntity(0.5f, 0.5f, Vector2(x+0.25f,y-0.25f)); break;
    case BL: entity = new WallEntity(0.5f, 0.5f, Vector2(x-0.25f,y+0.25f)); break;
    case BR: entity = new WallEntity(0.5f, 0.5f, Vector2(x+0.25f,y+0.25f)); break;
    default: LOGE("Unhandled tile type :%i", t); break;
  }
}

Tile::~Tile () {
  delete entity;
  LIST_FOREACH(const Trigger*, triggers, i) {
    delete (*i);
  }
}

Level::~Level() {
  for (unsigned x=0; x<width; x++) {
    for (unsigned y=0; y<height; y++)
      delete board[x][y];
    delete[] board[x];
  }
  delete bounds;
  delete[] tanks;
}

void Level::_initBoard (unsigned w, unsigned h, eTileType* b, bool* breakable, TankDescription* UNUSED(tanks), size_t numTanks, const list<TriggerDesc>& triggers) {
  //first entry in tank description must be player
  ASSERT(numTanks >= 1);
  width = w;
  height = h;
  board = new Tile**[width];
  for (unsigned x=0; x<width; x++)
    board[x] = new Tile*[height];

  for (unsigned y=0; y<height; y++) {
    for (unsigned x=0; x<width; x++) {
      //b is row-major, board is col-major
      const eTileType t = b[y*w+x];
      board[x][y] = new Tile(x, y, t);
      //checking for entity is just some sanity check
      if (board[x][y]->getEntity() && breakable[y*w+x])
        board[x][y]->getEntity()->setBreakable(true);
    }
  }

  //Add triggers to corresponding tiles
  LIST_FOREACH_CONST(TriggerDesc, triggers, i) {
    const TriggerDesc& d = *i;
    board[d.x][d.y]->addTrigger(d.trigger);
    triggeredTiles.append(board[d.x][d.y]);
  }
}

void Level::addToColManager(CollisionManager& colManager) {
  for (unsigned y = 0; y < height; y++) {
    for (unsigned x = 0; x < width; x++) {
      if (board[x][y]->getEntity())
        colManager.addEntity(board[x][y]->getEntity());
    }
  }
}

void Level::removeExplodedWalls (CollisionManager& colManager) {
  //FIXME: put exploded walls in a list instead of polling ??
  for (unsigned x=0; x<width; x++) {
    for (unsigned y=0; y<height; y++) {
      if (board[x][y]->getEntity() && board[x][y]->getEntity()->hasExploded()) {
        colManager.removeEntity(board[x][y]->getEntity());
        board[x][y]->deleteEntity();
      }
    }
  }
}
