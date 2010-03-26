#ifndef BCIRCLE_H_
#define BCIRCLE_H_

#include "logic/Entity.h"
#include "BoundingVolume.h"

class BCircle : public BoundingVolume {
  public:
    BCircle (float radius, Entity* e) :
      BoundingVolume(), radius(radius), entity(e) {
    }

    BVolumeType getType() const {
      return TYPE_CIRCLE;
    }

    float getWidth () const {
      return radius*2;
    }

    float getHeight () const {
      return radius*2;
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
