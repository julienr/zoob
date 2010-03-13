#ifndef MATH_H_
#define MATH_H_

#include <math.h>

#define RAD_TO_DEG(x) (x*180.0f/M_PI)

#define EPSILON 10E-5
struct Math {
  static bool epsilonEq (float x, float y) {
    return fabs(x-y) < EPSILON;
  }

  template <class T>
  static T signum (T n) {
    if (n < 0) return -1;
    if (n > 0) return 1;
    return 0;
  }
};

#endif /* MATH_H_ */
