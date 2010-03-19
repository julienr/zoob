#ifndef COLLISIONMANAGER_H_
#define COLLISIONMANAGER_H_

#include "lib/Vector2.h"
#include "logic/Entity.h"
#include "containers/utlist.h"
#include "def.h"


struct CollisionResult {
  float tFirst, tLast;
  Vector2 normal;
  Entity* collidedEntity;
};

class CollisionManager {
  public:
    CollisionManager ()
      : entities(NULL) {}

    void addEntity (Entity* e) {
      assert(e);
      EntityNode* n = new EntityNode(e);
      DL_APPEND(entities, n);
    }

    void unmarkCollided () {
      EntityNode* n;
      DL_FOREACH(entities, n) {
        n->entity->collided = false;
      }
    }

    void foreachEntity (void (*callback) (Entity*)) const {
      EntityNode* n;
      DL_FOREACH(entities, n) {
        callback(n->entity);
      }
    }

    //Perform a trace and put the result in "result"
    //returns false if no collision occurs
    //if false is returned, the content of result is undetermined
    bool trace (Entity* mover, const Vector2& end, CollisionResult& result);

  private:
    //FIXME: use grid to store entities
    struct EntityNode {
      EntityNode (Entity* e)
        : entity(e),prev(NULL),next(NULL) {}
      Entity* entity;
      EntityNode *prev;
      EntityNode *next;
    };

    EntityNode* entities;

};

#endif /* COLLISIONMANAGER_H_ */
