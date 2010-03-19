#include "BoundingBox.h"
#include "Entity.h"

Vector2 BoundingBox::getXAxis() const {
  //x' = x*cos(a) - y*sin(a)
  //y' = y*cos(a) + x*sin(a)
  const float r = entity->getRotation();
  return Vector2(cos(r), sin(r));
}

Vector2 BoundingBox::getYAxis() const {
  const float r = entity->getRotation();
  return Vector2(-sin(r), cos(r));
}

void BoundingBox::getCorners(Vector2(&c)[4]) const {
  const Vector2 tmp[2];
  //calculate half vectors
  tmp[0] = getXAxis() * getWidth() / 2.0f;
  tmp[1] = getYAxis() * getHeight() / 2.0f;
  //calculate (unrotated) center position)
  const Vector2 center = entity->getPosition() + Vector2(getWidth() / 2.0f,
      getHeight() / 2.0f);
  c[0] = center + tmp[0] + tmp[1];
  c[1] = center + tmp[0] - tmp[1];
  c[2] = center - tmp[0] - tmp[1];
  c[3] = center - tmp[0] + tmp[1];
}

Vector2 BoundingBox::getPosition() const {
  return entity->getPosition();
}
