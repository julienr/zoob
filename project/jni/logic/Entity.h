#ifndef ENTITY_H_
#define ENTITY_H_

#include "physics/BoundingVolume.h"
#include "Moveable.h"
#include "Viewable.h"
#include "containers/list.h"

enum eEntityType {
  ENTITY_TANK,
  ENTITY_ROCKET,
  ENTITY_WALL
};

class GridCell;

class Entity: public Moveable {
  friend class Grid;
  public:
    Entity(const BoundingVolume* v) :
      Moveable(), collided(false), bvolume(v) {
    }

    virtual ~Entity () {
      delete bvolume;
    }

    virtual eEntityType getType () const = 0;

    //Callback when this entity is subject to an explosion
    virtual void explode () = 0;

    const BoundingVolume* getBVolume() const {
      return bvolume;
    }

    float getWidth() const {
      return bvolume->getWidth();
    }

    float getHeight() const {
      return bvolume->getHeight();
    }

    //If an entity is not solid, it won't be considered in collision detection
    virtual bool isSolid () const {
      return true;
    }

    //FIXME: only for debug
    bool collided;
  private:
    //Used by Grid for performance reason. Entries equals to NULL are unused
    list<GridCell*> touchedCells;
    const BoundingVolume* bvolume;
};

#endif /* ENTITY_H_ */
