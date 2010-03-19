#include "CollisionManager.h"
#include "lib/Math.h"

#define FILL_PROJ_MIN_MAX(min,max,rect) float min = axis[i]*rect[0]; \
    float max = axis[i]*rect[0]; \
    for(short j=1;j<4;j++) { \
      float tmp = axis[i]*rect[j]; \
      if (tmp<min) \
        min = tmp; \
      if (tmp>max) \
        max = tmp; \
    }


bool MovingOBBAgainstOBB (const BoundingBox& still, const BoundingBox& moving, const Vector2& end, CollisionResult& r) {
  r.tFirst = 0.0f;
  r.tLast = MOOB_INF;
  const float tMax = 1.0f;
  const Vector2 vel = end-moving.getPosition();

  Vector2 axis[4];
  //FIXME: simplify if 2 AABB
  axis[0] = still.getXAxis();
  axis[1] = still.getYAxis();
  axis[2] = moving.getXAxis();
  axis[3] = moving.getYAxis();

  Vector2 corners[2][4];
  still.getCorners(corners[0]);
  moving.getCorners(corners[1]);

  //SAT
  for (int i=0; i<4; i++) {
    const float speed = axis[i]*vel;

    //Projected min/max on axis for both boxes
    FILL_PROJ_MIN_MAX(min0,max0,corners[0])
    FILL_PROJ_MIN_MAX(min1,max1,corners[1])

    if (max1 < min0) { //1 is on lhs of 0
      if (speed <= 0) return false;
      float T = (min0-max1)/speed;
      if (T > tMax) return false;
      if (T > r.tFirst) {
        r.tFirst = T;
        r.normal = Vector2(-axis[i].x, -axis[i].y);
      }
      T = (max0-min1)/speed;
      if (T < r.tLast) r.tLast = T;
      if (r.tFirst > r.tLast) return false;
    } else if (max0 < min1) { //1 is on rhs of 0
      if (speed >= 0) return false;
      float T = (max0-min1)/speed;
      if (T > tMax) return false;
      if (T > r.tFirst){
        r.tFirst = T;
        r.normal = Vector2(axis[i].x, axis[i].y);
      }
      T = (min0-max1)/speed;
      if (T < r.tLast) r.tLast = T;
      if (r.tFirst > r.tLast) return false;
    } else { //boxes overlapping
      if (speed >= 0) {
        float T = (max0-min1)/speed;
        if (T < r.tLast) {
          r.tLast = T;
          r.normal = Vector2(-axis[i].x, -axis[i].y);
        }
        if (r.tFirst > r.tLast) return false;
      } else { //speed < 0
        float T = (min0-max1)/speed;
        if (T < r.tLast) {
          r.tLast = T;
          r.normal = Vector2(axis[i].x, axis[i].y);
        }
        if (r.tFirst > r.tLast) return false;
      }
    }
  }
  return true;
}

bool collide (Entity* still, Entity* mover, const Vector2& end, CollisionResult& result) {
  if (MovingOBBAgainstOBB(still->getBBox(), mover->getBBox(), end, result)) {
    result.collidedEntity = still;
    return true;
  } else
    return false;
}

bool CollisionManager::trace (Entity* mover, const Vector2& end, CollisionResult& result) {
  CollisionResult r;
  result.tFirst = MOOB_INF;
  bool collided = false;

  EntityNode *n;
  DL_FOREACH(entities, n) {
    if (n->entity == mover)
      continue;

    if (collide(n->entity, mover, end, r)
        && r.tFirst < result.tFirst) {
      result = r;
      collided = true;
    }
  }

  return collided;
}
