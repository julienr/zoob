#ifndef ENTITY_H_
#define ENTITY_H_

#include "physics/BoundingVolume.h"
#include "Moveable.h"
#include "Viewable.h"

class Entity: public Moveable {
  public:
    Entity(const BoundingVolume* v) :
      Moveable(), collided(false), bvolume(v) {
    }

    virtual ~Entity () {
      delete bvolume;
    }

    const BoundingVolume* getBVolume() const {
      return bvolume;
    }

    float getWidth() const {
      return bvolume->getWidth();
    }

    float getHeight() const {
      return bvolume->getHeight();
    }

    //FIXME: only for debug
    bool collided;
  private:
    const BoundingVolume* bvolume;
};

#endif /* ENTITY_H_ */
