#ifndef LEVEL_H_
#define LEVEL_H_

#include "def.h"
#include "Entity.h"
#include "physics/CollisionManager.h"
#include "physics/AABBox.h"
#include "containers/vector.h"

enum eTileType {E, W, S}; //Empty, Wall, Spawn

class Level {
  public:
    //Since we can't pass a static 2D array as a func argument, board should be a pointer to the first element
    //and array subscripting is done our way then
    Level (unsigned w, unsigned h, eTileType* board) : startPositions(5) {
      _initBoard(w,h,board);
    }

    ~Level () {
      for (unsigned x=0; x<width; x++)
        delete[] board[x];
      delete[] board;
    }

    unsigned getHeight () const {
      return height;
    }

    unsigned getWidth () const {
      return width;
    }

    eTileType getTile (unsigned x, unsigned y) const {
      return board[x][y];
    }

    void addToColManager (CollisionManager& colManager);

    size_t getNumStartPositions () const {
      return startPositions.length();
    }

    Vector2 getStartPosition (size_t i) const {
      return startPositions[i];
    }

  private:
    void _initBoard (unsigned w, unsigned h, eTileType* board);
    unsigned width;
    unsigned height;
    eTileType** board;
    vector<Vector2> startPositions;
};

#endif /* LEVEL_H_ */
