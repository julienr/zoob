#include "SmartPolicy.h"
#include "logic/VisibilityGrid.h"
#include "logic/EnemyTank.h"
#include "logic/Game.h"

#define MAX_MODE_TIME 5

struct StopAtVisible : public AStar::StopCondition {
  const VisibilityGrid& vgrid;

  StopAtVisible (Game* game)
    : vgrid (game->getPlayerVisibility()) {}

  bool stopAt (const AStar::Cell* c) {
    return vgrid.getVisibility(c->x, c->y) == VISIBLE;
  }
};

bool SmartPolicy::decideDir (double elapsedS, Vector2* outDir, Game* game, EnemyTank* me) {
  modeElapsedS += elapsedS;

  //This mode switching algorithms will maximize fire and the tank is hidding only when he can't fire
  if (mode == AGGRESSIVE) {
    if (!me->canFire())
      switchMode(DEFENSIVE);
  } else {
    if (me->canFire())
      switchMode(AGGRESSIVE);
  }

  //FIXME: first move should be to avoid rockets

  int destX, destY;
  Path* p = NULL;
  if (mode == AGGRESSIVE) {
    //Aggressive : move towards player an stop at the first visible cell (this will allow us to fire)
    AStar* astar = game->getAStar();
    StopAtVisible cond(game);
    p = astar->shortestWay(me->getPosition(), game->getPlayerTank()->getPosition(), me, &cond);
  } else {
    //Defensive : move to the center of the biggest shadowed part of the map
    if (TankAI::rocketNear(game, me, 2 * GRID_CELL_SIZE, NULL))
      me->cancelFiring();

    VisibilityGrid& vgrid = game->getPlayerVisibility();
    bool found = vgrid.findCenterBiggestHidden(me, destX, destY);
    if (found) {
      //Find the path using AStar
      AStar* astar = game->getAStar();
      const Grid& grid = game->getColManager().getGrid();
      p = astar->shortestWay(me->getPosition(), grid.gridToWorld(destX, destY), me);
    }
  }

  if (!p) {
    //LOGE("No path found");
    return false;
  }

  game->dbg_addCellOverlay(CellOverlay(destX, destY, WHITE));
  game->dbg_addDebugPath(new DebugPath(p, VIOLET));

  //If tank is already at destination grid cell, don't move forward
  const Grid& g = game->getColManager().getGrid();
  const Vector2& pos = me->getPosition();
  if (g.getCellX(pos) == destX && g.getCellY(pos) == destY) {
    return false;
  }

  //LOGE("p(0) (%f,%f), tank position (%f,%f)", p->get(0).x, p->get(0).y, me->getPosition().x, me->getPosition().y);
  const Vector2 dir = (p->get(0) - me->getPosition()).getNormalized();
  outDir->set(dir);
  delete p;
  return true;
}

