#include "PathPolicy.h"
#include "logic/EnemyTank.h"
#include "ai/algorithms/AStar.h"
#include "logic/Game.h"

//How often we will test for give up on our path
#define GIVE_UP_INTERVAL 1000

//We give up if we have travalled at only 1/4 of our max speed
const float giveUpDist = TANK_MOVE_SPEED/(4*(1000.0f/GIVE_UP_INTERVAL));

bool PathPolicy::decideDir (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank) {
  Path* path = tank->getPath();
  if (!path)
    return false;

  //If a rocket is near the tank, cancel all firing and just run away
  //FIXME: shouldn't we delegate that to a third policy or to the firing policy ?
  /*if (rocketNear(game, tank,2*GRID_CELL_SIZE))
    tank->cancelFiring();*/

  //Stop moving while the tank is preparing to fire
  if (tank->isPreparingFire()) {
    prevSec = Utils::getCurrentTimeMillis();
    prevSecPos = tank->getPosition();
    return false;
  }

  //Did we arrive to the waypoint ?
  if (tank->getPosition() == path->get(current)) {
    current = (current+1)%path->length();
  }

  //Here, we have a "give up condition" on the waypoint.
  //If during a given interval, we have only travelled a given amount that is small,
  //this means we are probably stuck in some kind of race condition with other tanks
  const uint64_t now = Utils::getCurrentTimeMillis();
  if (now - prevSec > GIVE_UP_INTERVAL) {
    const float distCovered = (tank->getPosition()-prevSecPos).length();

    //LOGE("distCovered : %f, tank move speed : %f, giveup dist %f", distCovered, TANK_MOVE_SPEED, giveUpDist);
    if (distCovered < giveUpDist) {
      //LOGE("giving up on waypoint %i, heading to next", current);
      current = (current+1)%path->length();
    }
    prevSecPos = tank->getPosition();
    prevSec = now;
  }

  AStar* astar = game->getAStar();
  Path* shortestWay = astar->shortestWay(tank->getPosition(), path->get(current), tank);
  if (!shortestWay) {
    //cannot find a way, we're either on the waypoint or we cannot reach it => skip
    //We might be on the same logical cell as dest, but not be quite at dest yet
    if ((tank->getPosition() - path->get(current)).length() > 0.1) {
      Vector2* nodes = new Vector2[1];
      nodes[0] = path->get(current);
      shortestWay = new Path(1, nodes);
    } else {
      //skip to next
      current = (current+1)%path->length();
      return false;
    }
  }

  //game->dbg_addDebugPath(new DebugPath(shortestWay, RED));

  Vector2 dir = shortestWay->get(0) - tank->getPosition();
  outDir->set(dir.getNormalized());
  delete shortestWay;
  return true;
}
