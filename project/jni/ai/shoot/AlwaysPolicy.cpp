#include "AlwaysPolicy.h"

#include "logic/Game.h"
#include "logic/EnemyTank.h"

bool AlwaysPolicy::decideFire (double UNUSED(elapsedS), Vector2* outDir, Game* game, EnemyTank* tank) {
  PlayerTank* playerTank = game->getPlayerTank();
  if (playerTank) {
    *outDir = (playerTank->getPosition() - tank->getPosition()).getNormalized();
    return true;
  } else
    return false;
}

bool AlwaysPolicy::confirmFire (double UNUSED(elapsedS), Vector2* outDir, Game* game, EnemyTank* tank) {
  PlayerTank* playerTank = game->getPlayerTank();
  if (playerTank) {
    *outDir = (playerTank->getPosition() - tank->getPosition()).getNormalized();
    return true;
  } else
    return false;
}
