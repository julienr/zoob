#include "BoundingBox.h"
#include "Entity.h"
#include "def.h"

Vector2 BoundingBox::getXAxis() const {
  //x' = x*cos(a) - y*sin(a)
  //y' = y*cos(a) + x*sin(a)
  const float r = entity->getRotation();
  return Vector2(cosf(r), sinf(r));
}

Vector2 BoundingBox::getYAxis() const {
  const float r = entity->getRotation();
  return Vector2(-sinf(r), cosf(r));
}

void BoundingBox::getCorners(Vector2(&c)[4]) const {
  //calculate half vectors
  const Vector2 a1 = getXAxis()*(getWidth()*0.5f);
  const Vector2 a2 = getYAxis()*(getHeight()*0.5f);
  //(unrotated) center position)
  const Vector2 center = entity->getPosition();
  c[0] = center + a1 + a2;
  c[1] = center + a1 - a2;
  c[2] = center - a1 - a2;
  c[3] = center - a1 + a2;
}

Vector2 BoundingBox::getPosition() const {
  return entity->getPosition();
}
