#include "Level.h"

Tile::Tile(int x, int y, eTileType t): type(t) {
  switch(t) {
    case W: entity = new WallEntity(1.0f, 1.0f, Vector2(x,y)); break;
    case E: case S: entity = NULL; break;
    case T: entity = new WallEntity(1.0f, 0.5f, Vector2(x,y-0.25f)); break;
    case L: entity = new WallEntity(0.5f, 1.0f, Vector2(x-0.25f,y)); break;
    case B: entity = new WallEntity(1.0f, 0.5f, Vector2(x,y+0.25f)); break;
    //FIXME: have to use 0.24f instead of 0.25f to avoid rendering glitches...
    case R: entity = new WallEntity(0.5f, 1.0f, Vector2(x+0.24f,y)); break;
    case _1: case_2: entity = NULL; break;
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

void Level::_initBoard (unsigned w, unsigned h, eTileType* b) {
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
      if (t == S)
        playerStartPosition.set(x,y);
    }
  }
}

void Level::addToColManager(CollisionManager& colManager) {
  for (unsigned y = 0; y < height; y++) {
    for (unsigned x = 0; x < width; x++) {
      if (board[x][y]->getEntity())
        colManager.addEntity(board[x][y]->getEntity());
        //colManager.setGridCellSolid(x,y,true);
    }
  }
}
