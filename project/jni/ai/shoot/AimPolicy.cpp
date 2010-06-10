#include "AimPolicy.h"

#include "logic/Game.h"
#include "logic/EnemyTank.h"

bool AimPolicy::aim (double elapsedS, Game* game, EnemyTank* myTank, Vector2* outDir) {
  //FIXME: remove 3 lines
  CollisionResult r;
  const Vector2 dirToTank = game->getPlayerTank()->getPosition()-myTank->getPosition();
  game->getColManager().traceCircle(myTank, myTank->getPosition(), dirToTank, ROCKET_BCIRCLE_R, &r, ENTITY_ROCKET);
  *outDir = game->getPlayerTank()->getPosition() - myTank->getPosition();
  return true;
}

bool AimPolicy::decideFire (double elapsedS, Vector2* outDir, Game* game, EnemyTank* myTank) {
  CollisionResult r;
  const Vector2& tP = myTank->getPosition();

  //First, fire to approaching rockets
  Vector2 nearRocketPos;
  if (TankAI::rocketNear(game, myTank,4*GRID_CELL_SIZE, &nearRocketPos)) {
    //LOGE("rocket near");
    const Vector2 dirToRocket = nearRocketPos-tP;
    if (!game->getColManager().traceCircle(myTank, tP, dirToRocket, ROCKET_BCIRCLE_R, &r, ENTITY_ROCKET)) {
      outDir->set(dirToRocket.getNormalized());
      return true;
    }
  }

  //If we reach this point,no approaching rockets, try to fire to player
  const Vector2 dirToTank = game->getPlayerTank()->getPosition()-tP;
  if (doTraceCircle(game, myTank, tP, dirToTank, &r) && r.collidedEntity != game->getPlayerTank()) {
    //Cannot see, don't fire
    return false;
  } else {
    //LOGE("see player tank");
    outDir->set(dirToTank.getNormalized());
    return true;
  }
}

bool AimPolicy::doTraceCircle (Game* game, Entity* source, const Vector2& start, const Vector2& move, CollisionResult* result) {
  if (useBounceTrace)
    return game->getColManager().traceBounceCircle(source, start, move, ROCKET_BCIRCLE_R, result, ENTITY_ROCKET, ROCKET_MAX_BOUNCES);
  else
    return game->getColManager().traceCircle(source, start, move, ROCKET_BCIRCLE_R, result, ENTITY_ROCKET);
}

bool AimPolicy::confirmFire (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank) {
  //First, fire to approaching rockets
  Vector2 nearRocketPos;
  if (TankAI::rocketNear(game, tank,4*GRID_CELL_SIZE, &nearRocketPos)) {
    //LOGE("confirm rocket near");
    const Vector2 dirToRocket = nearRocketPos-tank->getPosition();
    CollisionResult r;
    if (!doTraceCircle(game, tank, tank->getPosition(), dirToRocket, &r)) {
      outDir->set(dirToRocket.getNormalized());
      return true;
    }
  }

  const Vector2 dirToTank = game->getPlayerTank()->getPosition()-tank->getPosition();
  outDir->set(dirToTank.getNormalized());
  return true;
}
