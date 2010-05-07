#include "AggressivePolicy.h"
#include "ai/algorithms/AStar.h"
#include "logic/Game.h"

bool AggressivePolicy::decideDir (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank) {
  AStar astar(game->getColManager().getGrid());
  Path* shortestWay = astar.shortestWay(tank->getPosition(), game->getPlayerTank().getPosition());
  if (!shortestWay)
    return false;
  Vector2 dir = shortestWay->get(0) - tank->getPosition();
  outDir->set(dir.getNormalized());
  return true;
}
