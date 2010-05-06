#include "Tank.h"
#include "Rocket.h"
#include "Bomb.h"

Rocket* Tank::fireRocket (Vector2 dir) {
  ASSERT(dir.length() > 0);
  dir.normalize();
  //Have to spawn the rocket just outside of our bounding box
  const Vector2 trans = dir*ROCKET_BCIRCLE_R + dir*TANK_BCIRCLE_R;
  setRotationFromDir(dir);
  firePolicy->fire();
  return new Rocket(this, getPosition()+trans*1.1, dir);
}

Bomb* Tank::dropBomb () {
  numMines++;
  minePolicy->fire();
  return new Bomb(this, getPosition());
}

void Tank::explode (Entity* e, const Vector2& dir) {
  //Avoid a tank being exploded by his own rocket
  //FIXME: really ?
  /*if (e && e->getType() == ENTITY_ROCKET) {
    Rocket* r = static_cast<Rocket*>(e);
    if (r->getOwner() == this) {
      LOGE("tank exploded by his own rocket => discards");
      return;
    }
  }*/
  exploded = true;
  LOGE("OMG, got an explosion");
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
