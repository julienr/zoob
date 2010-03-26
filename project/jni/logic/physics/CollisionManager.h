#ifndef COLLISIONMANAGER_H_
#define COLLISIONMANAGER_H_

#include "lib/Vector2.h"
#include "logic/Entity.h"
#include "containers/utlist.h"
#include "def.h"
#include "BCircle.h"
#include "AABBox.h"
#include "Grid.h"
#include "CollisionResult.h"

class CollisionManager {
  public:
    CollisionManager (int width, int height, float cellSize)
      : entities(NULL), grid(Vector2(-TILE_SIZE/2, -TILE_SIZE/2),width,height,cellSize) {}

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
      grid.clearTouched();
    }

    void foreachEntity (void (*callback) (Entity*)) const {
      EntityNode* n;
      DL_FOREACH(entities, n) {
        callback(n->entity);
      }
    }

    const Grid& getGrid () const {
      return grid;
    }

    //Perform a trace and put the result in "result"
    //move is move vector relative to mover
    //returns false if no collision occurs
    //if false is returned, the content of result is undetermined
    bool trace (Entity* mover, const Vector2& move, CollisionResult* result);

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

    Grid grid;
};

#endif /* COLLISIONMANAGER_H_ */
