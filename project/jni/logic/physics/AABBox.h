#ifndef AABBOX_H_
#define AABBOX_H_

#include "lib/Vector2.h"
#include "BoundingVolume.h"

//Axis-aligned bounding box
class AABBox: public BoundingVolume {
  public:
    AABBox(float w, float h) :
      BoundingVolume(), width(w), height(h) {
    }

    BVolumeType getType() const {
      return TYPE_AABBOX;
    }

    void getCorners(const Vector2& pos, Vector2(&c)[4]) const;

    float getWidth() const {
      return width;
    }

    float getHeight() const {
      return height;
    }
  private:
    const float width;
    const float height;
};

#endif /* BOUDINGBOX_H_ */
