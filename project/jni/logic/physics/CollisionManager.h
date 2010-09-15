#ifndef COLLISIONMANAGER_H_
#define COLLISIONMANAGER_H_

#include "lib/Vector2.h"
#include "lib/Polygon.h"
#include "logic/Entity.h"
#include "containers/list.h"
#include "def.h"
#include "BCircle.h"
#include "AABBox.h"
#include "Grid.h"
#include "CollisionResult.h"

class CollisionManager {
  public:
    static bool MovingAABBAgainstAABB (const Vector2& stillPos,
                                           const AABBox* still,
                                           const Vector2& movingPos,
                                           const AABBox* moving,
                                           const Vector2& move,
                                           CollisionResult* r);

    static bool MovingCircleAgainstAABB (const Vector2& stillPos,
                                              const AABBox* still,
                                              const Vector2& movingPos,
                                              const BCircle* moving,
                                              const Vector2& move,
                                              CollisionResult* r);

    static bool MovingCircleAgainstCircle (const Vector2& stillPos,
                                                const BCircle* still,
                                                const Vector2& movingPos,
                                                const BCircle* moving,
                                                const Vector2& move,
                                                CollisionResult* r);

    static bool MovingAgainstStill (const Vector2& stillPos,
                                        const BoundingVolume* still,
                                        const Vector2& movingPos,
                                        const BoundingVolume* moving,
                                        const Vector2& mode,
                                        CollisionResult* r);

    static bool LineAgainstAABB (const Vector2& boxPos,
                                    const AABBox* box,
                                    const Vector2& lineStart,
                                    const Vector2& lineMove,
                                    CollisionResult* r);

    static bool LineAgainstCircle (const Vector2& circlePos,
                                       const BCircle* circle,
                                       const Vector2& lineStart,
                                       const Vector2& lineMove,
                                       CollisionResult* r);

    static bool AABBIntersectCircle (const Vector2& boxPos,
                                     const AABBox* box,
                                     const Vector2& circlePos,
                                     const BCircle* circle);

    static bool CircleIntersectCircle (const Vector2& c1Pos,
                                       const BCircle* c1,
                                       const Vector2& c2Pos,
                                       const BCircle* c2);

    /*static bool PolyAgainstPoly (const Polygon& p1,
                                 const Polygon& p2);*/

    CollisionManager (int width, int height, float cellSize)
      : grid(Vector2(-TILE_SIZE/2, -TILE_SIZE/2),width,height,cellSize) {}

    void addEntity (Entity* e) {
      ASSERT(e);
      entities.append(e);
      grid.addEntity(e);
    }

    void removeEntity (Entity* e) {
      ASSERT(e);
      entities.remove(e);
      grid.removeEntity(e);
    }

    void unmarkCollided () {
      for (list<Entity*>::iterator i = entities.begin(); i.hasNext(); i++)
        (*i)->collided = false;
      grid.dbg_clearTouched();
      grid.unmarkCollided();
    }

    void foreachEntity (void (*callback) (Entity*)) const {
      for (list<Entity*>::const_iterator i = entities.begin(); i.hasNext(); i++)
        callback(*i);
    }

    const Grid& getGrid () const {
      return grid;
    }

    //Calls e->translate and modify the entities
    void translate (Entity* e, const Vector2& move) {
      e->translate(move);
      grid.moveEntity(e, move);
    }

    //Perform a trace and put the result in "result"
    //move is move vector relative to mover
    //returns false if no collision occurs
    //if false is returned, the content of result is undetermined
    //entityMask can be used to mask out some entities type from the trace
    //for example (0 ~ ENTITY_ROCKET) as an entityMask will make the trace ignore rockets
    bool trace (Entity* mover, const Vector2& move, CollisionResult* result, int entityMask=0) const;
    bool traceRay (Entity* source, const Vector2& start, const Vector2& move, CollisionResult* result, int entityMask=0) const;
    bool traceCircle (Entity* source, const Vector2& start, const Vector2& move, float radius, CollisionResult* result, int entityMask=0) const;

  private:
    //FIXME: use grid to store entities
    list<Entity*> entities;

    Grid grid;
};

#endif /* COLLISIONMANAGER_H_ */
