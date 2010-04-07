#ifndef LEVEL_H_
#define LEVEL_H_

#include "def.h"
#include "Entity.h"
#include "physics/CollisionManager.h"
#include "physics/AABBox.h"
#include "containers/vector.h"

class WallEntity : public Entity {
  public:
    WallEntity (float w, float h, const Vector2& pos) : Entity(new AABBox(w, h, this), pos) {}
    eEntityType getType () const { return ENTITY_WALL; }
    void explode () {}
};

/**
 * E: empty tile
 * W: wall (full cell size)
 * S: spawn
 * T: top (top half of cell)
 * L: left (left half of cell)
 * B: bottom (bottom half of cell)
 * R: right (right half of cell)
 */
enum eTileType {E, W, S, T, L, B, R};

class Tile {
  public:
    Tile(int x, int y, eTileType t);
    ~Tile () {
      if (entity)
        delete entity;
    }

    eTileType getType () { return type; }

    //Returns associated entity, MIGHT be NULL if this tile is not solid
    Entity* getEntity () { return entity; }
  private:
    eTileType type;
    Entity* entity;
};

class Level {
  public:
    //Since we can't pass a static 2D array as a func argument, board should be a pointer to the first element
    //and array subscripting is done our way then
    Level (unsigned w, unsigned h, eTileType* board) : startPositions(5) {
      _initBoard(w,h,board);
    }

    ~Level ();

    unsigned getHeight () const {
      return height;
    }

    unsigned getWidth () const {
      return width;
    }

    Tile* getTile (unsigned x, unsigned y) const {
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
    //eTileType** board;
    Tile*** board;
    vector<Vector2> startPositions;
};

#endif /* LEVEL_H_ */
