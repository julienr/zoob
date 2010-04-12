#include "AimPolicy.h"

#include "logic/Game.h"
#include "logic/Tank.h"

Vector2 AimPolicy::aim (double elapsedS, Game* game, Tank* tank) {
  return game->getPlayerTank().getPosition() - tank->getPosition();
}
