#include "GreenTank.h"
#include "Entity.h"

void GreenTank::explode (Entity* e, const Vector2& colPoint) {
  EnemyTank::explode(e, colPoint);
  //FIXME: actually, the rocket should BOUNCE against the shield

  //The shield of the green tank protects him of ROCKET explosions happening in front of him (90 degree FOV)
  if (e && e->getType() == ENTITY_ROCKET) {
    Vector2 t = colPoint - this->getPosition();
    t.normalize();
    const float colAngle = acos(t*Vector2::Y_AXIS) * Vector2::Y_AXIS.relativeOrientation(t);
    const float shieldMinAngle = this->getRotation() - M_PI/4;
    const float shieldMaxAngle = this->getRotation() + M_PI/4;
    //LOGE("rotation : %f, colAngle : %f, shieldMinAngle : %f, shieldMaxAngle : %f", this->getRotation(), colAngle, shieldMinAngle, shieldMaxAngle);
    if (Math::angleInInterval(colAngle, shieldMinAngle, shieldMaxAngle)) {
      //LOGE("shield !");
      this->unmarkExploded();
    }
  }
}
