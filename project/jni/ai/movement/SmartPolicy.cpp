#include "SmartPolicy.h"
#include "ai/algorithms/VisibilityGrid.h"
#include "logic/EnemyTank.h"
#include "logic/Game.h"

bool SmartPolicy::decideDir (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank) {
   //If a rocket is near the tank, cancel all firing and just run away
   //FIXME: shouldn't we delegate that to a third policy or to the firing policy ?
   if (rocketNear(game, tank,2*GRID_CELL_SIZE))
     tank->cancelFiring();

   //Stop moving while the tank is preparing to fire
   /*if (tank->isPreparingFire())
     return false;*/


   VisibilityGrid vgrid(game->getColManager().getGrid());
   vgrid.calculateVisibility(game);
   vgrid.djikstra(tank->getPosition(), tank);
   //vgrid.print();

   Path* shortestWay = vgrid.pathToClosestHidden();
   if (!shortestWay)
     return false;

   Vector2 dir = shortestWay->get(0) - tank->getPosition();
   outDir->set(dir.getNormalized());

   //FIXME: even if we shouldn't move, we should move to avoid rockets
   return true;
}

