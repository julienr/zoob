#include "RandomPolicy.h"

bool RandomPolicy::decideFire (double elapsedS, Vector2* outDir, Game* game, Tank* tank) {
  timeSinceFire += elapsedS;
  angle += elapsedS*FIRE_ROTATION_SPEED;
  if (timeSinceFire > FIRE_INTERVAL) {
    timeSinceFire = 0;
    outDir->set(cosf(angle), sinf(angle));
    return true;
  }
  return false;
}

Vector2 RandomPolicy::aim (double elapsedS, Game* game, Tank* tank) {
  return Vector2(cosf(angle), sinf(angle));
}
