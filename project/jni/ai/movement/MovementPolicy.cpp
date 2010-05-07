#include "MovementPolicy.h"
#include "logic/EnemyTank.h"
#include "logic/Game.h"


bool MovementPolicy::rocketNear (Game* game, EnemyTank* tank, float radius) {
  list<Entity*>* entities = game->getColManager().getGrid().entitiesIn(tank->getPosition(), radius);
  LIST_FOREACH(Entity*, (*entities), iter) {
     if ((*iter)->getType() == ENTITY_ROCKET) {
       return true;
     }
  }

  return false;
}
