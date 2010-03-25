#ifndef LEVEL_H_
#define LEVEL_H_

#include "def.h"
#include "Entity.h"
#include "CollisionManager.h"

enum eTileType {E, W, S}; //Empty, Wall, Spawn

/* Level tile */
class Tile: public Entity {
  public:
    Tile(int x, int y, eTileType type) :
      Entity(), type(type) {
      setPosition(Vector2(x, y));
    }

    eTileType getContentType() const {
      return type;
    }
  private:
    eTileType type;
};

class Level {
  public:
    //Since we can't pass a static 2D array as a func argument, board should be a pointer to the first element
    //and array subscripting is done our way then
    Level (unsigned w, unsigned h, eTileType* board) {
      _initBoard(w,h,board);
    }

    ~Level () {
      for (unsigned x=0; x<width; x++) {
        for (unsigned y=0; y<height; y++)
          delete board[x][y];
        delete[] board[x];
      }
      delete[] board;
    }

    unsigned getHeight () const {
      return height;
    }

    unsigned getWidth () const {
      return width;
    }

    const Tile* getTile (unsigned x, unsigned y) const {
      return board[x][y];
    }

    void addToColManager (CollisionManager& colManager);

    Vector2 getStartPosition () {
      //FIXME: return random start position and check if there already is someone ?
      for (unsigned x=0; x<width; x++) {
        for (unsigned y=0; y<height; y++) {
          if (board[x][y]->getContentType() == S)
            return Vector2(x,y);
        }
      }
      LOGE("getStartPosition : couldn't find a starting position");
      return Vector2(0,0);
    }

  private:
    void _initBoard (unsigned w, unsigned h, eTileType* board);
    unsigned width;
    unsigned height;
    Tile*** board;
};

#endif /* LEVEL_H_ */
