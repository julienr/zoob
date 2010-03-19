#ifndef BOUDINGBOX_H_
#define BOUDINGBOX_H_

#include "lib/Vector2.h"

class Entity;

//Oriented bounding box (automatically orient itself by using Entity's rotation
class BoundingBox {
public:
  BoundingBox (float w, float h, Entity* e) :
    width(w), height(h), entity(e) {
  }

  Vector2 getXAxis () const;
  Vector2 getYAxis () const;
  void getCorners (Vector2 (&c) [4]) const;
  Vector2 getPosition () const;

  float getWidth () const {
    return width;
  }

  float getHeight () const {
    return height;
  }
private:
  float width, height;
  Entity* entity;
};

#endif /* BOUDINGBOX_H_ */
