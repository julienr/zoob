#ifndef COLLISIONMANAGER_H_
#define COLLISIONMANAGER_H_

#include "lib/Vector2.h"
#include "logic/Entity.h"
#include "containers/utlist.h"


class CollisionResult {
  public:
    float getTFirst () const {
      return tFirst;
    }
    float getTLast () const {
      return tLast;
    }
    const Vector2& getNormal () const {
      return normal;
    }
    Entity* getCollidedEntity () const {
      return collidedEntity;
    }
  private:
    float tFirst, tLast;
    Vector2 normal;
    Entity* collidedEntity;
};

class CollisionManager {
  public:

    void addEntity (Entity* e) {
      DL_PREPEND(entities, new EntityNode(e));
    }
  private:
    //FIXME: use grid to store entities
    struct EntityNode {
      EntityNode (Entity* e)
        : entity(e),prev(NULL),next(NULL) {}
      Entity* entity;
      EntityNode *prev,*next;
    };

    EntityNode* entities;

};

#endif /* COLLISIONMANAGER_H_ */
