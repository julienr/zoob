#ifndef MATH_H_
#define MATH_H_

#include <math.h>

#define RAD_TO_DEG(x) (x*180.0f/M_PI)
#define DEG_TO_RAD(x) (x*M_PI/180.0f)
#define MIN(x,y) (x<y)?x:y
#define MAX(x,y) (x>y)?x:y

#define MOOB_INF 10E6
#define INT_INF 100000

#define EPSILON 10E-5
struct Math {
  //FIXME: implement (more expensive cause induces a division) http://floating-point-gui.de/errors/comparison/
  //or paper "comparing floating-point numbers"
  static bool epsilonEq (float x, float y) {
    return fabs(x-y) < EPSILON;
  }

  template <class T>
  static T signum (T n) {
    if (n < 0) return -1;
    if (n > 0) return 1;
    return 0;
  }

  //http://www.gamedev.net/community/forums/topic.asp?topic_id=563986&whichpage=1&#3615497
  static float angleInInterval (float currentAngle, float rightAngle, float leftAngle) {
    if (leftAngle >= rightAngle)
      return (rightAngle <= currentAngle && currentAngle <= leftAngle);
    else
      return ( !(leftAngle < currentAngle && currentAngle < rightAngle) );
  }

  static float sin (float angle) {
    return sinf(angle);
  }

  static float cos (float angle) {
    return cosf(angle);
  }

  static float ceil (float x) {
    return ceilf(x);
  }
};

#endif /* MATH_H_ */
