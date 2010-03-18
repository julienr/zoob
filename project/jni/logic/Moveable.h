#ifndef MOVEABLE_H_
#define MOVEABLE_H_

#include "lib/Vector2.h"
#include "containers/utlist.h"
#include "Viewable.h"

class Moveable : public Viewable {
  public:
    Moveable () :
      Viewable(), position(0,0), rotation(0) {
    }

    virtual ~Moveable () {}

    void setRotation(float r) {
      rotation = r;
    }

    float getRotation() const {
      return rotation;
    }

    void setPosition (const Vector2& v) {
      position.set(v);
    }

    void translate (const Vector2& v) {
      position.set(position.x + v.x, position.y + v.y);
    }

    Vector2 getPosition () const {
      return position;
    }
  private:
    Vector2 position;
    float rotation; //relative to center
};

#endif /* MOVEABLE_H_ */
