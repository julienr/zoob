#include "PathPolicy.h"
#include "logic/EnemyTank.h"

bool PathPolicy::decideDir (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank) {
  Path* path = tank->getPath();
  if (!path)
    return false;

  //Stop moving while the tank is preparing to fire
  if (tank->isPreparingFire())
    return false;

  //Did we arrive to the waypoint
  if ((tank->getPosition() - path->waypoints[current]).length() < 0.1) {
    current = (current+1)%path->numNodes;
  }

  outDir->set((path->waypoints[current] - tank->getPosition()).getNormalized());
  return true;
}
