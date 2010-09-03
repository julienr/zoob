#include "RandomPolicy.h"

bool RandomPolicy::decideFire (double elapsedS, Vector2* outDir, Game* UNUSED(game), EnemyTank* UNUSED(tank)) {
  timeSinceFire += elapsedS;
  angle += elapsedS*FIRE_ROTATION_SPEED;
  if (timeSinceFire > FIRE_INTERVAL) {
    timeSinceFire = 0;
    outDir->set(cosf(angle), sinf(angle));
    return true;
  }
  return false;
}

bool RandomPolicy::aim (double UNUSED(elapsedS), Game* UNUSED(game), EnemyTank* UNUSED(tank), Vector2* outDir) {
  outDir->set(cosf(angle), sinf(angle));
  return true;
}
