#ifndef VIEWABLE_H_
#define VIEWABLE_H_

#include "lib/Vector2.h"

class Viewable {
  public:
    virtual ~Viewable();

    //width/height in game coords
    virtual float getWidth() const = 0;
    virtual float getHeight() const = 0;
    virtual Vector2 getPosition() const = 0;

    Vector2 getCenter() const {
      const Vector2 p = getPosition();
      const float w = getWidth();
      const float h = getHeight();
      return Vector2(p.x + w / 2.0f, p.y + h / 2.0f);
    }

    Vector2 getSize () const {
      return Vector2(getWidth(), getHeight());
    }
};

#endif /* VIEWABLE_H_ */
