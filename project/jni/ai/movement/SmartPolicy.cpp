#include "SmartPolicy.h"
#include "logic/VisibilityGrid.h"
#include "logic/EnemyTank.h"
#include "logic/Game.h"

#define MAX_MODE_TIME 5

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
  //FIXME: remove
  mode = DEFENSIVE;

  //FIXME: first move should be to avoid rockets

  int destX, destY;
  Path* p;
  if (mode == AGGRESSIVE)
    p = _aggressiveDir(elapsedS, outDir, game, me, destX, destY);
  else
    p = _defensiveDir(elapsedS, outDir, game, me, destX, destY);

  if (!p) {
    LOGE("No path found");
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

Path* SmartPolicy::_aggressiveDir(double UNUSED(elapsedS), Vector2* UNUSED(outDir), Game* game, EnemyTank* tank, int& outX, int& outY) {
  //Move to closest visible cell, this should trigger the firing policy to fire
  VisibilityGrid& vgrid = game->getPlayerVisibility();
  vgrid.djikstra(tank->getPosition(), tank);

  return vgrid.pathToClosest(true, outX, outY);
}

Path* SmartPolicy::_defensiveDir(double UNUSED(elapsedS), Vector2* UNUSED(outDir), Game* game, EnemyTank* tank, int& outX, int& outY) {
  //If a rocket is near the tank, cancel all firing and just run away
  //FIXME: shouldn't we delegate that to a third policy or to the firing policy ?
  if (TankAI::rocketNear(game, tank, 2 * GRID_CELL_SIZE, NULL))
    tank->cancelFiring();

  //Stop moving while the tank is preparing to fire
  /*if (tank->isPreparingFire())
   return false;*/

  VisibilityGrid& vgrid = game->getPlayerVisibility();
  vgrid.djikstra(tank->getPosition(), tank);

  //Path* shortestWay = vgrid.pathToClosest(false);
  /** If we are in a hidden group, we should go for the center of it. Otherwise, we should go
   * to the biggest (closest ?) hidden group
   */
  return vgrid.pathToCenterBiggestHidden(outX, outY);
}

