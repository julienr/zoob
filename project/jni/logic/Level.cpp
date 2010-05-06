#include "Level.h"

Tile::Tile(int x, int y, eTileType t): type(t) {
  switch(t) {
    case W: entity = new WallEntity(1, 1, Vector2(x,y)); break;
    case E: entity = NULL; break;
    case T: entity = new WallEntity(1, 0.5f, Vector2(x,y-0.25f)); break;
    case L: entity = new WallEntity(0.5f, 1, Vector2(x-0.25f,y)); break;
    case B: entity = new WallEntity(1, 0.5f, Vector2(x,y+0.25f)); break;
    //FIXME: have to use 0.24f instead of 0.25f to avoid rendering glitches...
    case R: entity = new WallEntity(0.5f, 1, Vector2(x+0.24f,y)); break;
    case M: entity = new WallEntity(0.5f, 0.5f, Vector2(x+0.25f,y+0.25f)); break;
  }
}

Level::~Level() {
  for (unsigned x=0; x<width; x++) {
    for (unsigned y=0; y<height; y++)
      delete board[x][y];
    delete[] board[x];
  }
  delete[] board;
}

void Level::_initBoard (unsigned w, unsigned h, eTileType* b, TankDescription* tanks, size_t numTanks) {
  //first entry in tank description must be player
  ASSERT(numTanks >= 1);
  ASSERT(tanks[0].tankType == TANK_PLAYER);
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
    }
  }
}

void Level::addToColManager(CollisionManager& colManager) {
  //LOGE("addToColManager");
  for (unsigned y = 0; y < height; y++) {
    for (unsigned x = 0; x < width; x++) {
      if (board[x][y]->getEntity())
        colManager.addWallFromPosition(board[x][y]->getEntity());
        //colManager.setGridCellSolid(x,y,true);
    }
  }
  //LOGE("addToColManager");
}
