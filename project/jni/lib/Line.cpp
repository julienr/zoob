#include "Line.h"


//Gives m and h for the equation y=m*x+h
//We have normal.x * x + normal.y * y + c = 0
//and want to transform into y = m*x + h
//hence, y = -normal.x/normal.y * x - c / normal.y
static inline float m (const Line& l) {
  return -l.getNormal().x/l.getNormal().y;
}

static inline float h (const Line& l) {
  return -l.getC()/l.getNormal().y;
}

eLineIntersect Line::intersect (const Line& other, Vector2* result) const {
  //intersection of two lines. For formula developement see
  //http://www.gamedev.net/community/forums/topic.asp?topic_id=346591&whichpage=1&#2266676
  //The system is :
  //a1x + b1y = d1
  //a2x + b2y = d2
  //Solving via Cramer's rul gives
  //x = (b2d1 - b1d2)/(a1b2 - a2b1)
  //y = (a1d2 - a2d1)/(a1b2 - a2b1)
  //(in our case d = -c)

  const float denom = (normal.x*other.normal.y-other.normal.x*normal.y);
  if (Math::epsilonEq(denom, 0)) {
    return c==other.c?INTERSECT_INFINITY:INTERSECT_NONE;
  } else {
    const float d = -c;
    const float od = -other.c;
    const float x_num = (other.normal.y*d-normal.y*od);
    const float y_num = (normal.x*od-other.normal.x*d);
    result->set(x_num/denom, y_num/denom);
    return INTERSECT_ONCE;
  }
}
