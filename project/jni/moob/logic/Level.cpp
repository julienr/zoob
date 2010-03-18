#include "Level.h"


void Level::_initBoard (unsigned w, unsigned h, eTileType* b) {
  width = w;
  height = h;
  board = new Tile**[width];
  for (int x=0; x<width; x++)
    board[x] = new Tile*[height];

  for (int y=0; y<height; y++) {
    for (int x=0; x<width; x++) {
      //b is row-major, board is col-major
      board[x][y] = new Tile(x,y,b[x*w+y]);
    }
  }
}

void Level::addToColManager(CollisionManager& colManager) {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      if (board[x][y]->getContentType() == W)
        colManager.addEntity(board[x][y]);
    }
  }
}
