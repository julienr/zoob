#include "RandomPolicy.h"

Vector2 RandomPolicy::aim (double elapsedS, Game* game, Tank* tank) {
  return Vector2(cosf(angle), sinf(angle));
}
