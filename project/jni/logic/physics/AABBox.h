#ifndef AABBOX_H_
#define AABBOX_H_

#include "lib/Vector2.h"
#include "BoundingVolume.h"

class Entity;

//Axis-aligned bounding box
class AABBox: public BoundingVolume {
  public:
    AABBox(float w, float h, Entity* e) :
      BoundingVolume(), width(w), height(h), entity(e) {
    }

    BVolumeType getType() const {
      return TYPE_AABBOX;
    }

    void getCorners(Vector2(&c)[4]) const;
    Vector2 getPosition() const;

    float getWidth() const {
      return width;
    }

    float getHeight() const {
      return height;
    }
  private:
    float width, height;
    Entity* entity;
};

#endif /* BOUDINGBOX_H_ */
