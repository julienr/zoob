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


/**
 * Fill r with the result of collision
 * Returns true on collision
 * If the boxes are overlapping, NO normal is set in r
 * If they aren't, a normal is set
 */
bool MovingOBBAgainstOBB (const BoundingBox& still, const BoundingBox& moving, const Vector2& move, CollisionResult& r) {
  r.tFirst = 0.0f;
  r.tLast = MOOB_INF;
  const Vector2 vel = move;

  Vector2 axis[4];
  //FIXME: simplify if 2 AABB
  axis[0] = still.getXAxis();
  axis[1] = still.getYAxis();
  axis[2] = moving.getXAxis();
  axis[3] = moving.getYAxis();

  Vector2 corners[2][4];
  still.getCorners(corners[0]);
  moving.getCorners(corners[1]);

  //LOGE("MovingOBBAgainstOBB");

  //SAT
  for (int i=0; i<4; i++) {
    const float speed = axis[i]*vel;

    //Projected min/max on axis for both boxes
    FILL_PROJ_MIN_MAX(min0,max0,corners[0])
    FILL_PROJ_MIN_MAX(min1,max1,corners[1])

    //LOGE("axis(%f,%f) min0=%f,max0=%f  min1=%f,max1=%f   speed=%f", axis[i].x, axis[i].y, min0, max0, min1, max1, speed);

    //0 is still
    if (max1 < min0) { //1 is on lhs of 0
      //LOGE("1 on lhs of 0");
      if (speed <= 0) return false;
      float T = (min0-max1)/speed;
      if (T > r.tFirst) {
        r.tFirst = T;
        r.normal = -axis[i];
      }
      T = (max0-min1)/speed;
      if (T < r.tLast) r.tLast = T;
      if (r.tFirst > r.tLast) return false;
    } else if (max0 < min1) { //1 is on rhs of 0
      //LOGE("1 on rhs of 0");
      if (speed >= 0) return false;
      float T = (max0-min1)/speed;
      if (T > r.tFirst){
        r.tFirst = T;
        r.normal = axis[i];
      }
      T = (min0-max1)/speed;
      if (T < r.tLast) r.tLast = T;
      if (r.tFirst > r.tLast) return false;
    } else { //boxes overlapping
      //LOGE("overlap");
      if (speed >= 0) {
        float T = (max0-min1)/speed;
        if (T < r.tLast) {
          r.tLast = T;
          //Cannot calculate normal on overlap (we dunno if this is the first axis of intersection)
        }
        if (r.tFirst > r.tLast) return false;
      } else { //speed < 0
        float T = (min0-max1)/speed;
        if (T < r.tLast) {
          r.tLast = T;
          //Cannot calculate normal on overlap (we dunno if this is the first axis of intersection)
        }
        if (r.tFirst > r.tLast) return false;
      }
    }
    //LOGE("COLLISION");
  }
  return (r.tFirst <= 1.0f);
}

bool collide (Entity* still, Entity* mover, const Vector2& move, CollisionResult& result) {
  if (MovingOBBAgainstOBB(still->getBBox(), mover->getBBox(), move, result)) {
    result.collidedEntity = still;
    return true;
  } else
    return false;
}

bool CollisionManager::trace (Entity* mover, const Vector2& move, CollisionResult& result) {
  CollisionResult r;
  result.tFirst = MOOB_INF;
  bool collided = false;

  EntityNode *n;
  DL_FOREACH(entities, n) {
    if (n->entity == mover)
      continue;

    if (collide(n->entity, mover, move, r)
        && r.tFirst < result.tFirst) {
      result = r;
      result.colPoint = mover->getPosition()+move*r.tFirst;
      collided = true;
    }
  }

  return collided;
}
