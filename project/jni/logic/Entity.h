#ifndef ENTITY_H_
#define ENTITY_H_

#include "physics/BoundingVolume.h"
#include "Moveable.h"
#include "Viewable.h"
#include "containers/list.h"

//Assign power of 2 so we can use masks
enum eEntityType {
  ENTITY_TANK=1<<0,
  ENTITY_ROCKET=1<<1,
  ENTITY_WALL=1<<2,
  ENTITY_BOMB=1<<3
};

class GridCell;

class Entity: public Moveable {
  friend class Grid;
  public:
    Entity(const BoundingVolume* v) :
      Moveable(), collided(false), bvolume(v), id(0) {
    }

    Entity(const BoundingVolume* v, const Vector2& pos) :
      Moveable(pos), collided(false), bvolume(v), id(0) {
    }

    virtual ~Entity () {
      delete bvolume;
    }

    void setID (uint16_t id) {
      this->id = id;
    }

    uint16_t getID() const {
      return id;
    }

    virtual eEntityType getType () const = 0;

    //Callback when this entity is subject to an explosion
    //other is the entity responsible for the explosion, MIGHT be NULL
    //Returns how much damage this entity has taken due to the explosion. If returned damage is 0, a "poof"
    //explosion will be displayed to the player
    virtual int explode (Entity* other, const Vector2& colPoint) = 0;

    //Called when an other entity should touch this. If this function returns false, the touch is simply ignored
    virtual bool acceptsTouch (Entity* UNUSED(other)) {
      return true;
    }

    /**
     * This function is called before a touch/explode can occur on both entities.
     * If it returns true, the "other" entity will bounce against this one instead
     * of touching it
     */
    virtual bool bounce (Entity* UNUSED(other), const Vector2& UNUSED(colPoint)) {
      return false;
    }

    const BoundingVolume* getBVolume() const {
      return bvolume;
    }

    virtual float getWidth() const {
      return bvolume->getWidth();
    }

    virtual float getHeight() const {
      return bvolume->getHeight();
    }

    //If an entity is not solid, it won't be considered in collision detection
    virtual bool isSolid () const {
      return true;
    }

    //FIXME: only for debug
    bool collided;
  private:
    //Used by Grid for performance reason.
    list<GridCell*> touchedCells;
    const BoundingVolume* bvolume;

    //this entity's id. Used for networking
    uint16_t id; 
};

#endif /* ENTITY_H_ */
