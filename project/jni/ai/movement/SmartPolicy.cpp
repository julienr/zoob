#include "SmartPolicy.h"
#include "logic/VisibilityGrid.h"
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


   VisibilityGrid& vgrid = game->getPlayerVisibility();
   vgrid.djikstra(tank->getPosition(), tank);
   //vgrid.print();

   //Path* shortestWay = vgrid.pathToClosestHidden();
   /** If we are in a hidden group, we should go for the center of it. Otherwise, we should go
    * to the biggest (closest ?) hidden group
    */
   Path* shortestWay = vgrid.pathToCenterBiggestHidden();
   if (!shortestWay)
     return false;

   Vector2 dir = shortestWay->get(0) - tank->getPosition();
   outDir->set(dir.getNormalized());

   //FIXME: even if we shouldn't move, we should move to avoid rockets

   delete shortestWay;
   return true;
}

