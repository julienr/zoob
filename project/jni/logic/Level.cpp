#include "Level.h"


void Level::_initBoard (unsigned w, unsigned h, eTileType* b) {
  width = w;
  height = h;
  board = new eTileType*[width];
  for (unsigned x=0; x<width; x++)
    board[x] = new eTileType[height];

  for (unsigned y=0; y<height; y++) {
    for (unsigned x=0; x<width; x++) {
      //b is row-major, board is col-major
      board[x][y] = b[y*w+x];
    }
  }
}

void Level::addToColManager(CollisionManager& colManager) {
  for (unsigned y = 0; y < height; y++) {
    for (unsigned x = 0; x < width; x++) {
      if (board[x][y] == W)
        colManager.setGridCellSolid(x,y,true);
    }
  }
}
