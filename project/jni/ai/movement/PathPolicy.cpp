#include "PathPolicy.h"
#include "logic/Tank.h"

bool PathPolicy::decideDir (double elapsedS, Vector2* outDir, Game* game, Tank* tank) {
  Path* path = tank->getPath();
  if (!path)
    return false;

  //Did we arrive to the waypoint
  if ((tank->getPosition() - path->waypoints[current]).length() < 0.1) {
    current = (current+1)%path->numNodes;
  }

  outDir->set((path->waypoints[current] - tank->getPosition()).getNormalized());
  return true;
}
