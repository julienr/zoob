#include "GuidedRocket.h"
#include "ai/algorithms/AStar.h"
#include "logic/Game.h"

void GuidedRocket::think (double elapsedS) {
  AStar astar(Game::getInstance()->getColManager().getGrid());
  Path* shortestWay = astar.shortestWay(getPosition(), target->getPosition());
  if (!shortestWay) {
    //cannot find a way, we're either on the waypoint or we cannot reach it => skip
    //We might be on the same logical cell as dest, but not be quite at dest yet
    LOGE("at dest");
    if ((getPosition() - target->getPosition()).length() > 0.1) {
      Vector2* nodes = new Vector2[1];
      nodes[0] = target->getPosition();
      shortestWay = new Path(1, nodes);
    } else {
      delete shortestWay;
      return;
    }
  }
  Vector2 d = shortestWay->get(0) - getPosition();
  setDir(d.getNormalized());
  delete shortestWay;
}
