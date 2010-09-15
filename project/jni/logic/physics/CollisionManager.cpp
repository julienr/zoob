#include "CollisionManager.h"
#include "lib/Math.h"

/**
 * Good resource : N collision detection tutorial
 * http://physics.hardwire.cz/mirror/html/n_tutor_a/N%20Tutorials%20-%20Collision%20Detection%20and%20Response.html
 */

static bool collideOnAxis (const Vector2& axis, float min0, float max0, float min1, float max1, float speed, CollisionResult* r) {
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

//static overlap test with backoff calculation
static bool overlapOnAxis (const Vector2& axis, float min0, float max0, float min1, float max1, Vector2* backoff) {
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

//static overlap test without backoff calculation
static bool overlapOnAxis (const Vector2& axis, float min0, float max0, float min1, float max1) {
  return !(max1 < min0 || min1 > max0);
}

bool CollisionManager::MovingAgainstStill (const Vector2& stillPos,
                                                const BoundingVolume* still,
                                                const Vector2& movingPos,
                                                const BoundingVolume* moving,
                                                const Vector2& move,
                                                CollisionResult* r) {
  const BVolumeType sT = still->getType();
  const BVolumeType mT = moving->getType();

  if (mT == TYPE_AABBOX && sT == TYPE_AABBOX)
    return MovingAABBAgainstAABB(stillPos,
                                  static_cast<const AABBox*>(still),
                                  movingPos,
                                  static_cast<const AABBox*>(moving),
                                  move,
                                  r);
  else if (mT == TYPE_CIRCLE && sT == TYPE_AABBOX)
    return MovingCircleAgainstAABB(stillPos,
                                    static_cast<const AABBox*>(still),
                                    movingPos,
                                    static_cast<const BCircle*>(moving),
                                    move,
                                    r);
  else if (mT == TYPE_CIRCLE && sT == TYPE_CIRCLE)
    return MovingCircleAgainstCircle(stillPos,
                                      static_cast<const BCircle*>(still),
                                      movingPos,
                                      static_cast<const BCircle*>(moving),
                                      move,
                                      r);
  else if (mT == TYPE_LINE && sT == TYPE_AABBOX)
    return LineAgainstAABB(stillPos, static_cast<const AABBox*>(still), movingPos, move, r);
  else if (mT == TYPE_LINE && sT == TYPE_CIRCLE)
    return LineAgainstCircle(stillPos, static_cast<const BCircle*>(still), movingPos, move, r);

  else {
    LOGE("Unhandled collision type : moving %i against still %i", mT, sT);
    ASSERT(false);
    return false;
  }
}

bool CollisionManager::LineAgainstCircle (const Vector2& circlePos,
                                               const BCircle* circle,
                                               const Vector2& lineStart,
                                               const Vector2& lineMove,
                                               CollisionResult* r) {
  r->tFirst = 0.0f;
  r->tLast = MOOB_INF;

  //LOGE("LineAgainstCircle");
  const Vector2 centerToCenter = circlePos - lineStart;

  //distance between circles
  const float dist = centerToCenter.length() - circle->getRadius();
  //Project move on centerToCenter and
  const float moveC = lineMove*(centerToCenter.getNormalized());

  if (dist <= 0) {
    //overlap
    r->tFirst = 0.0f;
    r->tLast = (dist+circle->getRadius())/moveC;
    return true;
  } else {
    //no overlap
    if (moveC > dist) {
      //collision
      r->normal = -centerToCenter.getNormalized();
      r->tFirst = dist/moveC;
      r->tLast = (dist+circle->getRadius())/moveC;
      return true;
    } else
      return false;
  }
}

#define FILL_PROJ_MIN_MAX(min,max,rect,num) float min = axis[i]*rect[0]; \
    float max = axis[i]*rect[0]; \
    for(short j=1;j<num;j++) { \
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
bool CollisionManager::MovingAABBAgainstAABB (const Vector2& stillPos,
                                                    const AABBox* still,
                                                    const Vector2& movingPos,
                                                    const AABBox* moving,
                                                    const Vector2& move,
                                                    CollisionResult* r) {
  r->tFirst = 0.0f;
  r->tLast = MOOB_INF;

  Vector2 axis[2];
  axis[0] = Vector2(1,0);
  axis[1] = Vector2(0,1);

  Vector2 corners[2][4];
  still->getCorners(stillPos, corners[0]);
  moving->getCorners(movingPos, corners[1]);

  //LOGE("MovingOBBAgainstOBB");

  //SAT
  for (int i=0; i<4; i++) {
    const float speed = axis[i]*move;

    //Projected min/max on axis for both boxes
    FILL_PROJ_MIN_MAX(min0,max0,corners[0],4)
    FILL_PROJ_MIN_MAX(min1,max1,corners[1],4)

    //LOGE("axis(%f,%f) min0=%f,max0=%f  min1=%f,max1=%f   speed=%f", axis[i].x, axis[i].y, min0, max0, min1, max1, speed);

    if (!collideOnAxis(axis[i], min0, max0, min1, max1, speed, r))
      return false;

    //LOGE("COLLISION");
  }
  return (r->tFirst <= 1.0f);
}

/* FIXME: untested
 * bool CollisionManager::PolyAgainstPoly (const Polygon& p1,
                                        const Polygon& p2) {
  const int num1 = p1.getNumSides();
  const int numAxis = num1+p2.getNumSides();
  //The separating axis are orthogonal to both polygon's normals
  Vector2* axis = new Vector2[numAxis];
  for (int i=0; i<num1; i++)
    axis[i] = p1.getSides()[i]->getNormal();
  for (int i=num1; i<numAxis; i++)
    axis[i] = p2.getSides()[i-num1]->getNormal();

  const int nv1 = p1.getNumVerts();
  const Vector2* v1 = p1.getVerts();

  const int nv2 = p2.getNumVerts();
  const Vector2* v2 = p2.getVerts();

  bool result = true;
  //SAT
  for (int i=0; i<numAxis; i++) {
    //Projected min/max on axis for both polys
    FILL_PROJ_MIN_MAX(min0,max0,v1,nv1)
    FILL_PROJ_MIN_MAX(min1,max1,v2,nv2)

    if (!overlapOnAxis(axis[i], min0, max0, min1, max1)) {
      result = false;
      break;
    }
  }

  delete [] axis;
  return result;
}*/

bool CollisionManager::LineAgainstAABB (const Vector2& boxPos,
                                        const AABBox* box,
                                        const Vector2& lineStart,
                                        const Vector2& lineMove,
                                        CollisionResult* r) {
  r->tFirst = 0.0f;
  r->tLast = MOOB_INF;

  //We have 4 axis of separation => the box axes
  //and the line direction plus an axis orthogonal to the line dir
  Vector2 axis[4];
  axis[0] = Vector2(1,0);
  axis[1] = Vector2(0,1);
  axis[2] = lineMove.getNormalized();
  axis[3] = Vector2(-axis[2].y, axis[2].x).getNormalized();

  Vector2 corners[4];
  box->getCorners(boxPos, corners);

  //LOGE("LineAgainstAABB");

  //SAT
  for (int i=0; i<4; i++) {
    const float speed = axis[i]*lineMove;

    //Projected min/max on axis for the boxe
    FILL_PROJ_MIN_MAX(min0,max0,corners,4)
    float min1 = lineStart*axis[i];
    float max1 = min1;

    //LOGE("axis(%f,%f) min0=%f,max0=%f  min1=%f,max1=%f   speed=%f", axis[i].x, axis[i].y, min0, max0, min1, max1, speed);

    if (!collideOnAxis(axis[i], min0, max0, min1, max1, speed, r))
      return false;

    //LOGE("COLLISION");
  }
  return (r->tFirst <= 1.0f);
}


bool CollisionManager::MovingCircleAgainstCircle (const Vector2& stillPos,
                                                        const BCircle* still,
                                                        const Vector2& movingPos,
                                                        const BCircle* moving,
                                                        const Vector2& move,
                                                        CollisionResult* r) {
  r->tFirst = 0.0f;
  r->tLast = MOOB_INF;

  //LOGE("CircleAgainstCircle still (%f,%f,r=%f) against moving (%f,%f,r=%f) with move (%f,%f)", still->getPosition().x, still->getPosition().y,
  //    still->getRadius(), moving->getPosition().x, moving->getPosition().y, moving->getRadius(), move.x, move.y);
  const Vector2 centerToCenter = stillPos - movingPos;

  //distance between circles
  const float dist = centerToCenter.length() - still->getRadius() - moving->getRadius();
  //Project move on centerToCenter and
  const float moveC = move*(centerToCenter.getNormalized());
  //LOGE("dist : %f, moveC : %f", dist, moveC);

  if (dist <= 0) {
    //overlap
    r->tFirst = 0.0f;
    r->tLast = (dist+still->getRadius())/moveC;
    return true;
  } else {
    //no overlap
    if (moveC > dist) {
      //collision
      //r->normal = -move.getNormalized();
      r->normal = -centerToCenter.getNormalized();
      r->tFirst = dist/moveC;
      r->tLast = (dist+still->getRadius())/moveC;
      return true;
    } else
      return false;
  }
}

//http://stackoverflow.com/questions/401847/circle-rectangle-collision-detection-intersection
bool CollisionManager::AABBIntersectCircle (const Vector2& boxPos,
                                            const AABBox* box,
                                            const Vector2& circlePos,
                                            const BCircle* circle) {
  const float circDistX = fabs(circlePos.x - boxPos.x - box->getWidth()/2);
  const float circDistY = fabs(circlePos.y - boxPos.y - box->getHeight()/2);
  if (circDistX > (box->getWidth()/2 + circle->getRadius()))
    return false;
  if (circDistY > (box->getHeight()/2 + circle->getRadius()))
    return false;

  if (circDistX <= (box->getWidth()/2))
    return true;
  if (circDistY <= (box->getHeight()/2))
    return true;

  const float cdx = circDistX - box->getWidth()/2;
  const float cdy = circDistY - box->getHeight()/2;
  const float cornerDist = cdx*cdx + cdy*cdy;
  return cornerDist <= circle->getRadius()*circle->getRadius();
}

bool CollisionManager::CircleIntersectCircle (const Vector2& c1Pos,
                                              const BCircle* c1,
                                              const Vector2& c2Pos,
                                              const BCircle* c2) {
   const float dist = (c1Pos-c2Pos).length();
   return (dist < c1->getRadius() || dist < c2->getRadius());
}

bool CollisionManager::MovingCircleAgainstAABB (const Vector2& stillPos,
                                                      const AABBox* still,
                                                      const Vector2& movingPos,
                                                      const BCircle* moving,
                                                      const Vector2& move,
                                                      CollisionResult* r) {
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
  still->getCorners(stillPos, corners);

  //Determine voronoi regions
  const Vector2& c = movingPos;
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
    FILL_PROJ_MIN_MAX(min0,max0,corners,4)
    //For circle, min/max is always position +- radius
    const float min1 = (movingPos*axis[i])-moving->getRadius();
    const float max1 = (movingPos*axis[i])+moving->getRadius();

    //LOGE("axis(%f,%f) min0=%f,max0=%f  min1=%f,max1=%f   speed=%f", axis[i].x, axis[i].y, min0, max0, min1, max1, speed);

    if (!collideOnAxis(axis[i], min0, max0, min1, max1, speed, r))
      return false;

    //LOGE("COLLISION");
  }
  return (r->tFirst <= 1.0f);
}

bool CollisionManager::trace (Entity* mover, const Vector2& move, CollisionResult* result, int entityMask) const {
  return grid.push(mover, move, result, entityMask);
}

bool CollisionManager::traceRay (Entity* source, const Vector2& start, const Vector2& move, CollisionResult* result, int entityMask) const {
  return grid.traceRay(source, start, move, result, entityMask);
}

bool CollisionManager::traceCircle (Entity* source, const Vector2& start, const Vector2& move, float radius, CollisionResult* result, int entityMask) const {
  return grid.traceCircle(source, start, move, radius, result, entityMask);
}
