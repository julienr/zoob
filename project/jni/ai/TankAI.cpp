#include "ai/TankAI.h"
#include "logic/Game.h"
#include "logic/EnemyTank.h"

bool TankAI::rocketNear (Game* game, EnemyTank* tank, float radius, Vector2* outRocketPos) {
  list<Entity*>* entities = game->getColManager().getGrid().entitiesIn(tank->getPosition(), radius);
  bool res = false;
  LIST_FOREACH(Entity*, (*entities), iter) {
     if ((*iter)->getType() == ENTITY_ROCKET) {
       const Rocket* r = static_cast<Rocket*>(*iter);
       //We don't care about friendly rockets since they have no effect on us
       if (r->getOwner()->getTankType() != TANK_PLAYER)
         continue;
       if (outRocketPos != NULL)
         *outRocketPos = r->getPosition();
       res = true;
       break;
     }
  }
  delete entities;
  return res;
}
