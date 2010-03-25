#ifndef CIRCLE_H_
#define CIRCLE_H_

#include "Entity.h"

class BoundingCircle {
  public:
    BoundingCircle (float radius, Entity* e) :
      radius(radius), entity(e) {
    }

    float getRadius () const {
      return radius;
    }

    const Vector2& getPosition () const {
      return entity->getPosition();
    }

  private:
    const float radius;
    Entity* entity;
};

#endif
