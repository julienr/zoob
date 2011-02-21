#ifndef LINE_H_
#define LINE_H_

#include "zoobdef.h"
#include "lib/Math.h"
#include "lib/Vector2.h"

enum eLineRelPos {
  IN_FRONT,
  ON_LINE,
  BEHIND
};

enum eLineIntersect {
  INTERSECT_NONE,
  INTERSECT_ONCE,
  INTERSECT_INFINITY
};

class Line {
  public:
    Line () {}

    Line (const Line& other)
      : p1(other.p1), normal(other.normal), c(other.c) {}

    static Line fromPoints (const Vector2& p1, const Vector2& p2) {
      const Vector2& d = (p1-p2).getNormalized();
      return Line(p1, Vector2(d.y, -d.x));
    }

    static Line fromPointAndNormal(const Vector2& p1, const Vector2& normal) {
      return Line(p1, normal.getNormalized());
    }

    //1 = in front, 0 = on line, -1 = behind
    eLineRelPos getRelativePosition (const Vector2& p) const {
      const int sig = Math::signum(normal*p + c);
      if (sig == 1)
        return IN_FRONT;
      else if (sig == 0)
        return ON_LINE;
      else
        return BEHIND;
    }

    const Vector2& getNormal () const {
      return normal;
    }

    float getC () const {
      return c;
    }

    float distance (const Vector2& p) const {
      return normal*p+c;
    }

    void flipNormal () {
      normal = -normal;
      c *= -1;
    }

    //Compute intersection. Returns true and set result to the intersection point on success, false otherwise
    eLineIntersect intersect (const Line& other, Vector2* result) const;

  protected:
    Line (const Vector2& p1, const Vector2& normal)
      : p1(p1), normal(normal), c(-(normal*p1)) {}

    Vector2 p1;
    Vector2 normal;
    float c;
};

#endif /* LINE_H_ */
