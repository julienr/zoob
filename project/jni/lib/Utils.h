#ifndef UTILS_H_
#define UTILS_H_

#include "def.h"
#include "Vector2.h"
#include "sys/time.h"
#include <stdlib.h>

struct Utils {
  //Determine if point is inside the box determined by its top-left pos and width/height
  static bool insideTL (const Vector2& boxPos, float width, float height, const Vector2& p) {
    //LOGE("boxPos : %f, %f, width=%f, height=%f\t\tp=(%f,%f)", boxPos.x, boxPos.y, width, height, p.x, p.y);
    return (p.x > boxPos.x && p.x < boxPos.x+width) &&
            (p.y > boxPos.y && p.y < boxPos.y+height);
  }

  //Determine if the point is inside the box determined by its center and size (width, height)
  static bool insideC (const Vector2& boxPos, const Vector2& size, const Vector2& p) {
    const float hW = size.x/2.0f;
    const float hH = size.y/2.0f;
    return (p.x > boxPos.x-hW && p.x < boxPos.x+hW) &&
            (p.y > boxPos.y-hH && p.y < boxPos.y+hH);
  }

  //Determine if p is in circle[center,radius]
  static bool inCircle (const Vector2& center, float radius, const Vector2& p) {
    return (p.x > center.x-radius && p.x < center.x+radius) &&
            (p.y > center.y-radius && p.y < center.y+radius);
  }

  static uint64_t getCurrentTimeMillis () {
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000 + tv.tv_usec/1000;
  }

  //rand in [0,1]
  static float frand () {
    return rand()/(float)RAND_MAX;
  }
};


#endif /* UTILS_H_ */
