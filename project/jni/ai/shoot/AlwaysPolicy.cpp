#include "AlwaysPolicy.h"

#include "logic/Game.h"
#include "logic/EnemyTank.h"

bool AlwaysPolicy::decideFire (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank) {
  *outDir = (Game::getInstance()->getPlayerTank()->getPosition() - tank->getPosition()).getNormalized();
  return true;
}

bool AlwaysPolicy::confirmFire (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank) {
  *outDir = (Game::getInstance()->getPlayerTank()->getPosition() - tank->getPosition()).getNormalized();
  return true;
}
