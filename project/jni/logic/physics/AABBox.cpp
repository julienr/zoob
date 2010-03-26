#include "AABBox.h"
#include "logic/Entity.h"
#include "def.h"

void AABBox::getCorners(Vector2(&c)[4]) const {
  //calculate half vectors
  const Vector2 a1(getWidth()*0.5f, 0);
  const Vector2 a2(0, getHeight()*0.5f);
  //(unrotated) center position)
  const Vector2 center = entity->getPosition();
  c[0] = center + a1 + a2;
  c[1] = center + a1 - a2;
  c[2] = center - a1 - a2;
  c[3] = center - a1 + a2;
}

Vector2 AABBox::getPosition() const {
  return entity->getPosition();
}
