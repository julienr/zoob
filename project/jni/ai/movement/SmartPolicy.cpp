#include "SmartPolicy.h"
#include "logic/VisibilityGrid.h"
#include "logic/EnemyTank.h"
#include "logic/Game.h"

#define MAX_MODE_TIME 5

struct StopAtVisible : public PathFinder::StopCondition {
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
    if (!me->canFire()) {
      switchMode(DEFENSIVE);
      planPath(game, me);
    }
  } else {
    if (me->canFire()) {
      switchMode(AGGRESSIVE);
      planPath(game, me);
    }
  }

  //FIXME: first move should be to avoid rockets

  PathFinder* pathFinder = game->getPathFinder();
  if (pathFinder->needsReplanning(me)) {
    planPath(game,me);
  }

  if (!shortestWay) {
    return adjustForFiring(outDir, game, me);
  }

#ifdef DEBUG
  //game->dbg_addCellOverlay(CellOverlay(destX, destY, WHITE));
  game->dbg_addDebugPath(new DebugPath(shortestWay, VIOLET));
#endif

  //If tank is already at destination grid cell, don't move forward
  /*const Grid& g = game->getColManager().getGrid();
  const Vector2& pos = me->getPosition();
  if (g.getCellX(pos) == destX && g.getCellY(pos) == destY) {
    return adjustForFiring(outDir, game, me);
  }*/

  //LOGE("p(0) (%f,%f), tank position (%f,%f)", p->get(0).x, p->get(0).y, me->getPosition().x, me->getPosition().y);
  const Vector2 dir = (shortestWay->get(0) - me->getPosition()).getNormalized();
  outDir->set(dir);
  return true;
}

void SmartPolicy::planPath (Game* game, EnemyTank* me) {
  PathFinder* pathFinder = game->getPathFinder();
  delete shortestWay;
  PlayerTank* playerTank = game->getPlayerTank();
  if (playerTank && mode == AGGRESSIVE) {
    //Aggressive : move towards player and stop at the first visible cell (this will allow us to fire)
    StopAtVisible cond(game);
    shortestWay = pathFinder->findPath(me->getPosition(), playerTank->getPosition(), me, &cond);
  } else {
    //Defensive : move to the center of the biggest shadowed part of the map
    if (TankAI::rocketNear(game, me, 2 * GRID_CELL_SIZE, NULL))
      me->cancelFiring();

    VisibilityGrid& vgrid = game->getPlayerVisibility();
    int destX, destY;
    bool found = vgrid.findCenterBiggestHidden(me, destX, destY);
    if (found) {
      const Grid& grid = game->getColManager().getGrid();
      shortestWay = pathFinder->findPath(me->getPosition(), grid.gridToWorld(destX, destY), me);
    } else {
      shortestWay = NULL;
    }
  }
}

bool SmartPolicy::adjustForFiring (Vector2* outDir, Game* game, EnemyTank* me) {
  const VisibilityGrid& vgrid = game->getPlayerVisibility();
  const Grid& grid = game->getColManager().getGrid();
  const Vector2& pos = me->getPosition();
  PlayerTank* playerTank = game->getPlayerTank();
  if (playerTank && vgrid.getVisibility(grid.getCellX(pos), grid.getCellY(pos)) == VISIBLE) {
    //If we reach this point,no approaching rockets, try to fire to player
    const Vector2 dirToTank = playerTank->getPosition()-pos;
    CollisionResult r;
    if (game->getColManager().traceCircle(me, pos, dirToTank, ROCKET_BCIRCLE_R, &r, ENTITY_ROCKET)
          && r.collidedEntity != playerTank) {
      //collided against something => move in the direction of the col normal
      outDir->set(r.normal);
      return true;
    }
  }
  return false;
}

