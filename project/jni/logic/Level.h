#ifndef LEVEL_H_
#define LEVEL_H_

#include "def.h"
#include "Entity.h"
#include "physics/CollisionManager.h"
#include "physics/AABBox.h"
#include "containers/vector.h"
#include "logic/Tank.h"

class WallEntity : public Entity {
  public:
    WallEntity (float w, float h, const Vector2& pos) : Entity(new AABBox(w-EPSILON, h-EPSILON), pos), width(w),height(h) {}
    eEntityType getType () const { return ENTITY_WALL; }
    void explode (Entity* e, const Vector2& colPoint) {}

    float getWidth() const {
      return width;
    }

    float getHeight() const {
      return height;
    }
  private:
    //We store our own width/height (used for display) separate from the width/height used for aabbox
    //The bbox will be an EPSILON smaller than the displayed box just to avoid that when the level is added
    //to colmanager, a level tile spawns 5 cells
    const float width;
    const float height;
};

/**
 * E: empty tile
 * W: wall (full cell size)
 * T: top (top half of cell)
 * L: left (left half of cell)
 * B: bottom (bottom half of cell)
 * R: right (right half of cell)
 * M: only small block in the middle
 */
enum eTileType {E, W, T, L, B, R, M};

class Tile {
  public:
    Tile(int x, int y, eTileType t);
    ~Tile () {
      delete entity;
    }

    eTileType getType () const { return type; }

    //Returns associated entity, MIGHT be NULL if this tile is not solid
    Entity* getEntity () const { return entity; }
  private:
    eTileType type;
    Entity* entity;
};

/* This structure is used to provide some "advanced" description for tanks such as
 * path to be followed */
struct TankDescription {
  TankDescription (int x, int y, eTankType t, Path* path)
    : x(x), y(y), tankType(t), path(path) {}
  int x, y; //tank start position
  eTankType tankType; //should be _1 or _2
  Path* path;
};

class Level {
  public:
    //Since we can't pass a static 2D array as a func argument, board should be a pointer to the first element
    //and array subscripting is done our way then
    //The tanks array MUST contain the player tank description in its first index
    Level (unsigned w, unsigned h, eTileType* board, TankDescription* tanks, size_t numTanks)
      : tanks(tanks), numTanks(numTanks), bounds(new AABBox(w, h)){
      _initBoard(w,h,board, tanks, numTanks);
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

    Vector2 getStartPosition () const {
      return Vector2(tanks[0].x, tanks[0].y);
    }

    const TankDescription* getTanks () const { return tanks; }
    size_t getNumTanks () const { return numTanks; }

    const AABBox* getBounds () const { return bounds; }
    const Vector2 getCenter () const { return Vector2(width/2.0f, height/2.0f); }

  private:
    void _initBoard (unsigned w, unsigned h, eTileType* board, TankDescription* tanks, size_t numTanks);
    unsigned width;
    unsigned height;
    //eTileType** board;
    Tile*** board;

    const TankDescription* tanks;
    const size_t numTanks;

    AABBox* bounds;
};

#endif /* LEVEL_H_ */
