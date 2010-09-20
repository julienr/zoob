#ifndef LEVEL_H_
#define LEVEL_H_

#include "app.h"
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
    bool explode (Entity* UNUSED(e), const Vector2& UNUSED(colPoint)) { return false; }

    float getWidth() const {
      return width;
    }

    float getHeight() const {
      return height;
    }

    bool bounce (Entity* other, const Vector2& UNUSED(colPoint)) {
      if (other->getType() == ENTITY_ROCKET)
        return true;
      else
        return false;
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
enum eTileType {
    E=1,
    W, //wall
    T, //hafl top
    L, //hafl left
    B, //half bottom
    R, //half right
    M, //middle
    TL, //top left corner
    TR, //top right corner
    BL, //bottom left corner
    BR, //bottom right corner
};

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
  TankDescription ()
    : x(-1), y(-1), tankType(TANK_PLAYER), path(NULL) {}
  TankDescription (int x, int y, eTankType t, Path* path)
    : x(x), y(y), tankType(t), path(path) {}
  int x, y; //tank start position
  eTankType tankType; //should be _1 or _2
  Path* path;
};

//items available in the level
enum eItems {
    ITEM_BOMB=1,
    ITEM_BOUNCE=2,
    ITEM_SHIELD=4,
    ITEM_FIRING=8
};

//reward screen shown at the end of the level
enum eReward {
    REWARD_NONE=0,
    REWARD_BOMB=MENU_LAST,
    REWARD_BOUNCE,
    REWARD_SHIELD,
    REWARD_FIRING
};

class Level {
  public:
    //Since we can't pass a static 2D array as a func argument, board should be a pointer to the first element
    //and array subscripting is done our way then

    //Level take ownership of the tanks array
    //board should be FREED BY CALLER
    Level (unsigned w, unsigned h, eTileType* board, TankDescription* tanks, size_t numTanks, bool drawShadows, bool boss, uint8_t items, eReward reward)
      : tanks(tanks), numTanks(numTanks), bounds(new AABBox(w, h)), drawShadows(drawShadows), boss(boss), reward(reward), items(items) {
      _initBoard(w,h,board, tanks, numTanks);
    }

    ~Level ();

    bool hasItem(eItems mask) const { return items & mask; }
    eReward getReward () const { return reward; }

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

    const TankDescription* getTanks () const { return tanks; }
    size_t getNumTanks () const { return numTanks; }

    const AABBox* getBounds () const { return bounds; }
    const Vector2 getCenter () const { return Vector2(width/2.0f, height/2.0f); }

    //wether the shadow effect should be used
    bool hasShadows () const { return drawShadows; }

    //wether the boss intro (??? sign) should be used
    bool isBoss () const { return boss; }

  private:
    void _initBoard (unsigned w, unsigned h, eTileType* board, TankDescription* tanks, size_t numTanks);
    unsigned width;
    unsigned height;
    //eTileType** board;
    Tile*** board;

    const TankDescription* tanks;
    const size_t numTanks;

    AABBox* bounds;
    bool drawShadows;
    bool boss;
    eReward reward;
    uint8_t items;
};

#endif /* LEVEL_H_ */
