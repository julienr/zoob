#ifndef VECTOR2_H_
#define VECTOR2_H_

#include <math.h>
#include "Math.h"

struct Vector2 {
  static const Vector2 X_AXIS;
  static const Vector2 Y_AXIS;
  static const Vector2 ZERO;

  float x, y;

  Vector2 () :
    x(0), y(0) {}

  Vector2 (float x, float y) :
    x(x), y(y) {}

  Vector2 (const Vector2& other) :
    x(other.x), y(other.y) {}

  void normalize () {
    const float l = length();
    x /= l;
    y /= l;
  }

  Vector2 getNormalized () const {
    const float l = length();
    return Vector2(x/l, y/l);
  }

  bool isZero () {
    return x == 0 && y == 0;
  }

  void set (float x, float y) {
    this->x = x;
    this->y = y;
  }

  void set (const Vector2& o) {
    set(o.x, o.y);
  }

  float length () const {
    return sqrt(x*x+y*y);
  }

  float operator * (const Vector2& other) const {
    return x*other.x + y*other.y;
  }

  bool operator == (const Vector2& other) const {
    return Math::epsilonEq(x, other.x) &&
            Math::epsilonEq(y, other.y);
  }

  bool operator != (const Vector2& other) const {
    return !(*this == other);
  }

  float relativeOrientation (const Vector2& other) const {
    return Math::signum(x*other.y - y*other.x);
  }

  Vector2 operator * (float c) const {
    return Vector2(x*c, y*c);
  }

  inline
  friend Vector2 operator * (float c, const Vector2& v);

  Vector2 operator / (float c) const {
    return Vector2(x/c, y/c);
  }

  Vector2 operator + (const Vector2& o) const {
    return Vector2(x+o.x, y+o.y);
  }

  void operator += (const Vector2& o) {
    x += o.x;
    y += o.y;
  }

  Vector2 operator - (const Vector2& o) const {
    return Vector2(x-o.x, y-o.y);
  }

  Vector2 operator - () const {
    return Vector2(-x,-y);
  }
};

Vector2 operator * (float c, const Vector2& v) {
  return v*c;
}

#endif /* VECTOR2_H_ */
