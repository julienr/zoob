#ifndef LEVEL_H_
#define LEVEL_H_

#include "app.h"
#include "def.h"
#include "Entity.h"
#include "physics/CollisionManager.h"
#include "physics/AABBox.h"
#include "containers/vector.h"
#include "containers/set.h"
#include "containers/pair.h"
#include "logic/Tank.h"

class Trigger;

class WallEntity : public Entity {
  public:
    WallEntity (float w, float h, const Vector2& pos) : Entity(new AABBox(w-EPSILON, h-EPSILON), pos), width(w),height(h), breakable(false), exploded(false) {}
    eEntityType getType () const { return ENTITY_WALL; }

    int explode (Entity* UNUSED(e), const Vector2& UNUSED(colPoint)) {
      exploded = true;
      return 1;
    }

    float getWidth() const {
      return width;
    }

    float getHeight() const {
      return height;
    }

    bool acceptsTouch (Entity* other) {
      if (breakable && other->getType() == ENTITY_ROCKET)
        return true;
      else
        return false;
    }

    bool bounce (Entity* other, const Vector2& UNUSED(colPoint)) {
      if (breakable)
        return false;

      if (other->getType() == ENTITY_ROCKET)
        return true;
      else
        return false;
    }

    void setBreakable (bool b) { breakable = b; }
    bool isBreakable () { return breakable; }

    bool hasExploded () const { return exploded; }

  private:
    //We store our own width/height (used for display) separate from the width/height used for aabbox
    //The bbox will be an EPSILON smaller than the displayed box just to avoid that when the level is added
    //to colmanager, a level tile spawns 5 cells
    const float width;
    const float height;

    bool breakable;
    bool exploded;
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
    ~Tile ();

    eTileType getType () const { return type; }

    //Returns associated entity, MIGHT be NULL if this tile is not solid
    WallEntity* getEntity () const { return entity; }

    void deleteEntity () { delete entity; entity = NULL; }

    void addTrigger (const Trigger* t) {
      triggers.append(t);
    }

    void removeTrigger (const Trigger* t) {
      triggers.remove(t);
    }

    const list<const Trigger*>& getTriggers () const {
      return triggers;
    }

  private:
    eTileType type;
    WallEntity* entity;
    list<const Trigger*> triggers;
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

//Struct used when loading triggers
struct TriggerDesc {
  TriggerDesc (const Trigger* trigger, const int x, const int y)
    : x(x), y(y), trigger(trigger) {}
  const int x;
  const int y;
  const Trigger* trigger;
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
    Level (unsigned w, unsigned h, eTileType* board, bool* breakable, TankDescription* tanks, size_t numTanks, const list<TriggerDesc>& triggers, bool drawShadows, bool boss, uint8_t items, eReward reward, const list<pair<int,int> >& spawns)
      : tanks(tanks), numTanks(numTanks), bounds(new AABBox(w, h)), drawShadows(drawShadows), boss(boss), reward(reward), items(items), spawnPositions(spawns) {
      _initBoard(w,h,board, breakable, tanks, numTanks, triggers);
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

    void removeExplodedWalls (CollisionManager& colManager);

    const TankDescription* getTanks () const { return tanks; }
    size_t getNumTanks () const { return numTanks; }

    const AABBox* getBounds () const { return bounds; }
    const Vector2 getCenter () const { return Vector2(width/2.0f, height/2.0f); }

    //wether the shadow effect should be used
    bool hasShadows () const { return drawShadows; }

    //wether the boss intro (??? sign) should be used
    bool isBoss () const { return boss; }

    void removeTrigger (Tile* tile, const Trigger* trigger) {
      tile->removeTrigger(trigger);
      if (tile->getTriggers().size() == 0)
        tilesWithTrigger.remove(tile);
    }

    const set<Tile*>& getTilesWithTrigger () {
      return tilesWithTrigger;
    }

    const list<pair<int,int> >& getSpawnPositions () {
      return spawnPositions;
    }

  private:
    void _initBoard (unsigned w, unsigned h, eTileType* board, bool* breakable, TankDescription* tanks, size_t numTanks, const list<TriggerDesc>& triggers);
    unsigned width;
    unsigned height;
    Tile*** board;

    const TankDescription* tanks;
    const size_t numTanks;

    AABBox* bounds;
    bool drawShadows;
    bool boss;
    eReward reward;
    uint8_t items;
    //This is just for fast access. Contains a pointer to ALL the tiles that have a trigger attached to them
    set<Tile*> tilesWithTrigger;

    //mostly for multiplayer, find a spawn position
    list<pair<int,int> > spawnPositions;
};

#endif /* LEVEL_H_ */
