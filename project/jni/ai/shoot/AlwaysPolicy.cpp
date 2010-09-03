#include "AlwaysPolicy.h"

#include "logic/Game.h"
#include "logic/EnemyTank.h"

bool AlwaysPolicy::decideFire (double UNUSED(elapsedS), Vector2* outDir, Game* UNUSED(game), EnemyTank* tank) {
  *outDir = (Game::getInstance()->getPlayerTank()->getPosition() - tank->getPosition()).getNormalized();
  return true;
}

bool AlwaysPolicy::confirmFire (double UNUSED(elapsedS), Vector2* outDir, Game* UNUSED(game), EnemyTank* tank) {
  *outDir = (Game::getInstance()->getPlayerTank()->getPosition() - tank->getPosition()).getNormalized();
  return true;
}
