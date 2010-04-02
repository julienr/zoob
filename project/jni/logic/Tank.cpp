#include "Tank.h"
#include "Rocket.h"

Rocket* Tank::fireRocket (const Vector2& dir) {
  //Have to spawn the rocket just outside of our bounding box
  const Vector2 trans = dir*ROCKET_BCIRCLE_R + dir*TANK_BCIRCLE_R;
  return new Rocket(this, getPosition()+trans*1.1, dir);
}
