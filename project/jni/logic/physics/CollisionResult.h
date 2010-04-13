#ifndef COLLISIONRESULT_H_
#define COLLISIONRESULT_H_

#include "lib/Vector2.h"
class Entity;

struct CollisionResult {
  float tFirst, tLast;
  Vector2 normal;
  Entity* collidedEntity;
  Vector2 colPoint;
};

#endif /* COLLISIONRESULT_H_ */
