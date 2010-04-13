#ifndef BCIRCLE_H_
#define BCIRCLE_H_

#include "BoundingVolume.h"

class BCircle : public BoundingVolume {
  public:
    BCircle (float radius) :
      BoundingVolume(), radius(radius){
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

  private:
    const float radius;
};

#endif
