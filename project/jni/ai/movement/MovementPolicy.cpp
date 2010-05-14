#include "MovementPolicy.h"
#include "logic/EnemyTank.h"
#include "logic/Game.h"


bool MovementPolicy::rocketNear (Game* game, EnemyTank* tank, float radius) {
  list<Entity*>* entities = game->getColManager().getGrid().entitiesIn(tank->getPosition(), radius);
  bool res = false;
  LIST_FOREACH(Entity*, (*entities), iter) {
     if ((*iter)->getType() == ENTITY_ROCKET) {
       res = true;
       break;
     }
  }
  delete entities;
  return res;
}
