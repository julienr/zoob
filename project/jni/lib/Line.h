#ifndef LINE_H_
#define LINE_H_

#include "def.h"
#include "lib/Math.h"
#include "lib/Vector2.h"

class Line {
  public:
    Line(const Vector2& p1, const Vector2& p2) : p1(p1) {
      const Vector2 v = p1-p2;
      normal.set(v.y, -v.x);
      normal.normalize();
      c = -(normal*p1);
    }

    //1 = in front, 0 = on line, -1 = behind
    int getRelativePosition (const Vector2& p) const {
      return Math::signum(normal*p + c);
    }

    const Vector2& getNormal () const {
      return normal;
    }

    void flipNormal () {
      normal = -normal;
      c *= -1;
    }

    void setNormal (const Vector2& n) {
      normal = n;
      c = -(normal*p1);
    }

  protected:
    const Vector2 p1;
    Vector2 normal;
    float c;
};

#endif /* LINE_H_ */
