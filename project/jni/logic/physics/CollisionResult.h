#ifndef COLLISIONRESULT_H_
#define COLLISIONRESULT_H_

struct CollisionResult {
  float tFirst, tLast;
  Vector2 normal;
  Entity* collidedEntity;
  Vector2 colPoint;
};

#endif /* COLLISIONRESULT_H_ */
