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
bool MovingOBBAgainstOBB (const BoundingBox& still, const BoundingBox& moving, const Vector2& move, CollisionResult* r) {
  r->tFirst = 0.0f;
  r->tLast = MOOB_INF;
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

    if (!collideOnAxis(axis[i], min0, max0, min1, max1, speed, r))
      return false;

    //LOGE("COLLISION");
  }
  return (r->tFirst <= 1.0f);
}

bool StaticCircleAgainstOBB (const BoundingCircle& circle, const BoundingBox& obb, Vector2* backoff) {
  backoff->set(MOOB_INF, MOOB_INF);
  //FIXME: use voronoi regions to only check 3 axis (see N tutorials)
  //We have to check the box axes and the axes going from circle center to each box corner
  Vector2 axis[6];
  axis[0] = obb.getXAxis();
  axis[1] = obb.getYAxis();

  Vector2 corners[4];
  obb.getCorners(corners);
  for (int i=2; i<6; i++)
    axis[i] = (corners[i-2] - circle.getPosition()).getNormalized();

  for (int i=0; i<6; i++) {
    //obb min/max
    FILL_PROJ_MIN_MAX(min0,max0,corners);
    //circle min/max on this axis, this is always position-radius, position+radius
    const float min1 = (circle.getPosition() - axis[i]*circle.getRadius())*axis[i];
    const float max1 = (circle.getPosition() + axis[i]*circle.getRadius())*axis[i];

    if (!overlapOnAxis(axis[i], min0, max0, min1, max1, backoff))
      return false;
  }
  return true;
}

bool collide (Entity* still, Entity* mover, const Vector2& move, CollisionResult* result) {
  if (MovingOBBAgainstOBB(still->getBBox(), mover->getBBox(), move, result)) {
    result->collidedEntity = still;
    return true;
  } else
    return false;
}

bool CollisionManager::rotationOverlap (Entity* mover, float angle, Vector2* backoff, Entity** touchedEntity) {
  backoff->set(MOOB_INF, MOOB_INF);
  bool collided = false;
  //We actually don't care about the angle, we just create a bounding circle (so we assume 2pi rotation)
  const BoundingBox& bbox = mover->getBBox();
  const float hW = bbox.getWidth()*0.5f;
  const float hH = bbox.getHeight()*0.5f;
  const float circleRadius = sqrt(hW*hW + hH*hH);
  BoundingCircle circle(circleRadius, mover);

  EntityNode *n;
  DL_FOREACH(entities, n) {
    if (n->entity == mover)
      continue;

    Vector2 tmp;
    if (StaticCircleAgainstOBB(circle, n->entity->getBBox(), &tmp)) {
      *touchedEntity = n->entity;
      if (tmp.length() < backoff->length())
        *backoff = tmp;
      collided = true;
    }
  }
  return collided;
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
      result->colPoint = mover->getPosition()+move*r.tFirst;
      collided = true;
    }
  }

  return collided;
}
