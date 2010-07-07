#include "Tank.h"
#include "Rocket.h"
#include "Bomb.h"
#include "physics/CollisionManager.h"

Rocket* Tank::fireRocket (Vector2 dir) {
  ASSERT(dir.length() > 0);
  dir.normalize();
  //Have to spawn the rocket just outside of our bounding box
  const Vector2 trans = dir*ROCKET_BCIRCLE_R + dir*tankRadius;
  setRotationFromDir(dir);
  forceDirTimer.start();
  firePolicy->fire();
  return createRocket(this, getPosition()+trans*1.5, dir);
}

bool Tank::checkFireDir (const Vector2& dir, const CollisionManager& colManager, CollisionResult* r) {
  const Vector2 trans = dir*ROCKET_BCIRCLE_R + dir*tankRadius;
  return !colManager.traceCircle(this, getPosition(), trans*1.5, ROCKET_BCIRCLE_R, r, 0);
}

Bomb* Tank::dropBomb () {
  numMines++;
  bombPolicy->fire();
  return new Bomb(this, getPosition());
}

bool Tank::explode (Entity* e, const Vector2& dir) {
  numLives--;
  if (numLives > 0)
    return true;

  exploded = true;
  LOGE("OMG, got an explosion");
  return true;
}

bool Tank::shieldBounce (Entity* e, const Vector2& colPoint) {
  //The shield of a tank protects him of ROCKET explosions happening in front of him (90 degree FOV)
  if (e && e->getType() == ENTITY_ROCKET) {
    Vector2 t = colPoint - this->getPosition();
    t.normalize();
    const float colAngle = acos(t*Vector2::Y_AXIS) * Vector2::Y_AXIS.relativeOrientation(t);
    const float shieldMinAngle = this->getRotation() - M_PI/4;
    const float shieldMaxAngle = this->getRotation() + M_PI/4;
    if (Math::angleInInterval(colAngle, shieldMinAngle, shieldMaxAngle)) {
      return true;
    }
  }
  return false;
}
