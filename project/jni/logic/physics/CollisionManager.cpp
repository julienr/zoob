#include "CollisionManager.h"
#include "lib/Math.h"

/**
 * Good resource : N collision detection tutorial
 * http://physics.hardwire.cz/mirror/html/n_tutor_a/N%20Tutorials%20-%20Collision%20Detection%20and%20Response.html
 */

bool collideOnAxis (const Vector2& axis, float min0, float max0, float min1, float max1, float speed, CollisionResult* r) {
  //0 is still
  if (max1 < min0) { //1 is on lhs of 0
    //LOGE("1 on lhs of 0");
    if (speed <= 0) return false;
    float T = (min0-max1)/speed;
    if (T > r->tFirst) {
      r->tFirst = T;
      r->normal = -axis;
    }
    T = (max0-min1)/speed;
    if (T < r->tLast) r->tLast = T;
    if (r->tFirst > r->tLast) return false;
  } else if (max0 < min1) { //1 is on rhs of 0
    //LOGE("1 on rhs of 0");
    if (speed >= 0) return false;
    float T = (max0-min1)/speed;
    if (T > r->tFirst){
      r->tFirst = T;
      r->normal = axis;
    }
    T = (min0-max1)/speed;
    if (T < r->tLast) r->tLast = T;
    if (r->tFirst > r->tLast) return false;
  } else { //boxes overlapping
    //LOGE("overlap");
    if (speed >= 0) {
      float T = (max0-min1)/speed;
      if (T < r->tLast) {
        r->tLast = T;
        //Cannot calculate normal on overlap (we dunno if this is the first axis of intersection)
      }
      if (r->tFirst > r->tLast) return false;
    } else { //speed < 0
      float T = (min0-max1)/speed;
      if (T < r->tLast) {
        r->tLast = T;
        //Cannot calculate normal on overlap (we dunno if this is the first axis of intersection)
      }
      if (r->tFirst > r->tLast) return false;
    }
  }
  return true;
}

bool overlapOnAxis (const Vector2& axis, float min0, float max0, float min1, float max1, Vector2* backoff) {
  //The two objects are separated on this axis
  if (max1 < min0 || min1 > max0)
    return false;

  if (min1 > min0) {
    const float amount = fabs(max0-min1);
    if (amount < backoff->length())
      backoff->set(axis*amount);
  } else { //max1 > min0
    const float amount = fabs(max1-min0);
    if (amount < backoff->length())
      backoff->set(axis*amount);
  }
  return true;
}

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
bool MovingAABBAgainstAABB (const AABBox* still, const AABBox* moving, const Vector2& move, CollisionResult* r) {
  r->tFirst = 0.0f;
  r->tLast = MOOB_INF;

  Vector2 axis[2];
  axis[0] = Vector2(1,0);
  axis[1] = Vector2(0,1);

  Vector2 corners[2][4];
  still->getCorners(corners[0]);
  moving->getCorners(corners[1]);

  //LOGE("MovingOBBAgainstOBB");

  //SAT
  for (int i=0; i<4; i++) {
    const float speed = axis[i]*move;

    //Projected min/max on axis for both boxes
    FILL_PROJ_MIN_MAX(min0,max0,corners[0])
    FILL_PROJ_MIN_MAX(min1,max1,corners[1])

    //LOGE("axis(%f,%f) min0=%f,max0=%f  min1=%f,max1=%f   speed=%f", axis[i].x, axis[i].y, min0, max0, min1, max1, speed);

    if (!collideOnAxis(axis[i], min0, max0, min1, max1, speed, r))
      return false;

    //LOGE("COLLISION");
  }
  return (r->tFirst <= 1.0f);
}


bool MovingCircleAgainstAABB (const AABBox* still, const BCircle* moving, const Vector2& move, CollisionResult* r) {
  r->tFirst = 0.0f;
  r->tLast = MOOB_INF;

  //FIXME : We have only 3 axis to check because we use Voronoi region to detect which box vertice is closest to the circle
  /**
   *           |    |
   *      .....------.....
   *           |    |
   *           |    |
   *      .....------.....
   *           |    |
   */
  Vector2 corners[4];
  still->getCorners(corners);

  Vector2 axis[6];
  axis[0] = Vector2(1,0);
  axis[1] = Vector2(0,1);
  for (int i=2; i<6; i++)
    axis[i] = (corners[i-2]-moving->getPosition()).getNormalized();

  //SAT
  for (int i=0; i<4; i++) {
    const float speed = axis[i]*move;

    //Projected min/max on axis for both boxes
    FILL_PROJ_MIN_MAX(min0,max0,corners)
    //For circle, min/max is always position +- radius
    const float min1 = (moving->getPosition()*axis[i])-moving->getRadius();
    const float max1 = (moving->getPosition()*axis[i])+moving->getRadius();

    //LOGE("axis(%f,%f) min0=%f,max0=%f  min1=%f,max1=%f   speed=%f", axis[i].x, axis[i].y, min0, max0, min1, max1, speed);

    if (!collideOnAxis(axis[i], min0, max0, min1, max1, speed, r))
      return false;

    //LOGE("COLLISION");
  }
  return (r->tFirst <= 1.0f);
}

bool collide (Entity* still, Entity* mover, const Vector2& move, CollisionResult* result) {
  const BoundingVolume* v1 = still->getBVolume();
  const BoundingVolume* v2 = mover->getBVolume();
  if (v1->getType() == TYPE_AABBOX) {
    if (v2->getType() == TYPE_CIRCLE)
      return MovingCircleAgainstAABB(static_cast<const AABBox*>(v1), static_cast<const BCircle*>(v2), move, result);
    else {
      LOGE("Unsupported collision detection type : moving aabbox against aabbox");
      assert(false);
    }
  } else if (v1->getType() == TYPE_CIRCLE) {
    LOGE("Unsupported collision detection type : still circle");
    assert(false);
  }
  return false;
}

bool CollisionManager::trace (Entity* mover, const Vector2& move, CollisionResult* result) {
  CollisionResult r;
  result->tFirst = MOOB_INF;
  bool collided = false;

  EntityNode *n;
  DL_FOREACH(entities, n) {
    if (n->entity == mover)
      continue;

    if (collide(n->entity, mover, move, &r)
        && r.tFirst < result->tFirst) {
      (*result) = r;
      result->collidedEntity = n->entity;
      result->colPoint = mover->getPosition()+move*r.tFirst;
      collided = true;
    }
  }

  return collided;
}
