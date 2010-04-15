#include "GreenTank.h"
#include "Entity.h"

bool GreenTank::bounce (Entity* e, const Vector2& colPoint) {
  //The shield of the green tank protects him of ROCKET explosions happening in front of him (90 degree FOV)
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
