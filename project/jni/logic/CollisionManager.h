#ifndef COLLISIONMANAGER_H_
#define COLLISIONMANAGER_H_

#include "lib/Vector2.h"
#include "logic/Entity.h"
#include "containers/utlist.h"
#include "def.h"
#include "BoundingCircle.h"


struct CollisionResult {
  float tFirst, tLast;
  Vector2 normal;
  Entity* collidedEntity;
  Vector2 colPoint;
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
    //move is move vector relative to mover
    //returns false if no collision occurs
    //if false is returned, the content of result is undetermined
    bool trace (Entity* mover, const Vector2& move, CollisionResult* result);

    //Test what the "mover" would collide if its rotation would be angle
    bool rotationOverlap (Entity* mover, float angle, Vector2* backoff, Entity** touchedEntity);

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
