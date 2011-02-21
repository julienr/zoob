#include "AABBox.h"
#include "logic/Entity.h"
#include "zoobdef.h"

/**
 *  0 --- 1
 *   |   |
 *  3 --- 2
 */
void AABBox::getCorners(const Vector2& center, Vector2(&c)[4]) const {
  //calculate half vectors
  const Vector2 a1(getWidth()*0.5f, 0);
  const Vector2 a2(0, getHeight()*0.5f);
  //(unrotated) center position)
  c[0] = center - a1 - a2;
  c[1] = center + a1 - a2;
  c[2] = center + a1 + a2;
  c[3] = center - a1 + a2;
}
