#include "PathPolicy.h"
#include "logic/EnemyTank.h"
#include "ai/algorithms/AStar.h"
#include "logic/Game.h"

bool rocketNear (Game* game, EnemyTank* tank) {
  list<Entity*>* entities = game->getColManager().getGrid().entitiesIn(tank->getPosition(), 2*GRID_CELL_SIZE);
  LIST_FOREACH(Entity*, (*entities), iter) {
     if ((*iter)->getType() == ENTITY_ROCKET) {
       return true;
     }
  }

  return false;
}

bool PathPolicy::decideDir (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank) {
  Path* path = tank->getPath();
  if (!path)
    return false;

  //If a rocket is near the tank, cancel all firing and just run away
  //FIXME: shouldn't we delegate that to a third policy or to the firing policy ?
  if (rocketNear(game, tank))
    tank->cancelFiring();

  //Stop moving while the tank is preparing to fire
  if (tank->isPreparingFire())
    return false;

  //Did we arrive to the waypoint
  if ((tank->getPosition() - path->get(current)).length() < 0.1) {
    current = (current+1)%path->length();
  }

  /*outDir->set((path->get(current) - tank->getPosition()).getNormalized());
  return true;*/

  AStar astar(game->getColManager().getGrid());
  Path* shortestWay = astar.shortestWay(tank->getPosition(), path->get(current));
  if (!shortestWay) {
    //cannot find a way, we're either on the waypoint or we cannot reach it => skip
    current = (current+1)%path->length();
    return false;
  }
  Vector2 dir = shortestWay->get(0) - tank->getPosition();
  outDir->set(dir.getNormalized());
  return true;
}
