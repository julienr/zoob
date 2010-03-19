#ifndef ENTITY_H_
#define ENTITY_H_

#include "BoundingBox.h"
#include "Moveable.h"
#include "Viewable.h"

class Entity: public Moveable {
  public:
    Entity() :
      Moveable(), collided(false), bbox(1.0f, 1.0f, this) {
    }

    virtual ~Entity () {}

    const BoundingBox& getBBox() const {
      return bbox;
    }

    float getWidth() const {
      return bbox.getWidth();
    }

    float getHeight() const {
      return bbox.getHeight();
    }

    //FIXME: only for debug
    bool collided;
  private:
    BoundingBox bbox;
};

#endif /* ENTITY_H_ */
