#include "Tank.h"
#include "Rocket.h"

Rocket* Tank::fireRocket (const Vector2& dir) {
  return new Rocket(this, getPosition(), dir);
}
