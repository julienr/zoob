#include "Level.h"

Tile::Tile(int x, int y, eTileType t): type(t) {
  //FIXME: We use 0.95f instead of 1.0f and 0.45f instead of 0.5f to avoid a block spanning on more than one grid
  //cell (due to rounding errors)
  switch(t) {
    case W: entity = new WallEntity(0.95f, 0.95f, Vector2(x,y)); break;
    case E: entity = NULL; break;
    case T: entity = new WallEntity(0.95f, 0.45f, Vector2(x,y-0.25f)); break;
    case L: entity = new WallEntity(0.45f, 0.95f, Vector2(x-0.25f,y)); break;
    case B: entity = new WallEntity(0.95f, 0.45f, Vector2(x,y+0.25f)); break;
    //FIXME: have to use 0.24f instead of 0.25f to avoid rendering glitches...
    case R: entity = new WallEntity(0.45f, 0.95f, Vector2(x+0.24f,y)); break;
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
        colManager.addEntity(board[x][y]->getEntity());
        //colManager.setGridCellSolid(x,y,true);
    }
  }
  //LOGE("addToColManager");
}
