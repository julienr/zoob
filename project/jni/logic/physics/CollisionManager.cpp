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


bool CollisionManager::MovingCircleAgainstAABB (const AABBox* still, const BCircle* moving, const Vector2& move, CollisionResult* r) {
  r->tFirst = 0.0f;
  r->tLast = MOOB_INF;

  //We have only 2 or 3 axis to check because we use Voronoi region to detect which box vertice is closest to the circle
  /**
   *   --- > x
   *   |    1  |  2 |  3
   *   |  .....------.....
   *   y       |    |
   *        4  |    | 5
   *      .....------.....
   *        6  | 7  | 8
   *
   *  If we are in region 1,3,6 or 8, we have to check (additionnaly to the box axis), the axis going from
   *  circle center to the nearest vertex. If we are in region 2,4,5,7, we just have to check 2 axis (the boxe's one)
   */
  Vector2 corners[4];
  still->getCorners(corners);

  //Determine voronoi regions
  const Vector2& c = moving->getPosition();
  Vector2* vertex = NULL;
  if (c.y < corners[0].y) { //region 1,2 or 3
    if (c.x < corners[0].x) //1
      vertex = &corners[0];
    else if (c.x < corners[1].x) // 2
      vertex = NULL;
    else //3
      vertex = &corners[1];
  } else if (c.y < corners[3].y) { //region 4 or 5
    vertex = NULL;
  } else { //region 6,7 or 8
    if (c.x < corners[3].x) // 6
      vertex = &corners[3];
    else if (c.x < corners[2].x) //7
      vertex = NULL;
    else //8
      vertex = &corners[2];
  }

  const unsigned numAxis = (vertex==NULL)?2:3;

  Vector2 axis[numAxis];
  axis[0] = Vector2(1,0);
  axis[1] = Vector2(0,1);
  if (vertex != NULL) {
    axis[2] = (*vertex - c).getNormalized();
  }

  //SAT
  for (unsigned i=0; i<numAxis; i++) {
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
      return CollisionManager::MovingCircleAgainstAABB(static_cast<const AABBox*>(v1), static_cast<const BCircle*>(v2), move, result);
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
  return grid.trace(static_cast<const BCircle*>(mover->getBVolume()), move, result);
  /*CollisionResult r;
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

  return collided;*/
}
