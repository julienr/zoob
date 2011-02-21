#ifndef GRID_H_
#define GRID_H_

#include "zoobdef.h"
#include "BCircle.h"
#include "CollisionResult.h"
#include "logic/Entity.h"
#include "AABBox.h"
#include "containers/list.h"
#include "containers/vector.h"

class GridCell {
public:
  GridCell (unsigned x, unsigned y) :
    x(x),y(y),dbgTouched(false){
  }

  const unsigned x;
  const unsigned y;
  bool dbgTouched;
  list<Entity*> entities;
};

class WallEntity;

class Grid {
  public:
    //We have to subdivise the area [width, height] in cells of size cellSize
    //Origin is the origin of the coordinate systems wolrd objects to be placed in the grid are in
    Grid (const Vector2& origin, unsigned w, unsigned h, float cellSize);
    ~Grid ();

    void addEntity (Entity* e);

    //It is ASSUMED that entity position is the one obtained after the last call to moveEntity
    void removeEntity (Entity* e);

    //Must be called when an entity has been moved. Will update its grid position
    void entityMoved (Entity* e);

    void dbg_clearTouched ();

    void unmarkCollided ();

    //DEBUG purposes only
    bool dbg_touched (unsigned x, unsigned y) const {
      if (!inside(x,y))
        return false;
      return grid[x][y]->dbgTouched;
    }

    //To be used for movement < cellSize
    bool push (const Entity* mover, const Vector2& move, CollisionResult* result, int entityMask) const;

    bool traceRay (const Entity* source, const Vector2& start, const Vector2& move, CollisionResult* result, int entityMask) const;
    bool traceCircle (Entity* source, const Vector2& start, const Vector2& move, float radius, CollisionResult* result, int entityMask) const;

    unsigned getWidth () const {
      return width;
    }

    unsigned getHeight () const {
      return height;
    }

    //returns -1 if outside grid
    int getCellX (const Vector2& point) const {
      return getCellX(point.x);
    }

    int getCellXBounded (const Vector2& point) const {
      return getCellXBounded(point.x);
    }

    int getCellX (float px) const {
      const int x = (int)((px-origin.x)/cellSize);
      if (x < 0 || x >= (int)width)
        return -1;
      return x;
    }

    int getCellXBounded (float px) const {
      const int x = (int)((px-origin.x)/cellSize);
      if (x < 0) return 0;
      if (x >= (int)width) return width-1;
      return x;
    }

    //returns -1 if outside grid
    int getCellY (const Vector2& point) const {
      return getCellY(point.y);
    }

    int getCellYBounded (const Vector2& point) const {
      return getCellYBounded(point.y);
    }

    int getCellY (float py) const {
      const int y = (int)((py-origin.y)/cellSize);
      if (y < 0 || y >= (int)height)
        return -1;
      return y;
    }

    int getCellYBounded (float py) const {
      const int y = (int)((py-origin.y)/cellSize);
      if (y < 0) return 0;
      if (y >= (int)height) return height-1;
      return y;
    }

    bool inside (int x, int y) const {
      return (x >= 0) && (x < (int)width) &&
              (y >= 0) && (y < (int)height);
    }

    float getCellSize () const {
      return cellSize;
    }

    //return the center of the cell [x][y], in world coordinates
    Vector2 gridToWorld (int x, int y) const {
      const float hcs = cellSize/2;
      return Vector2(x*cellSize + origin.x + hcs, y*cellSize + origin.y + hcs);
    }

    void capToGrid (Vector2* v) const {
      if (v->x < 0) v->x = 0.1f;
      if (v->y < 0) v->y = 0.1f;
      const float maxX = width*cellSize;
      const float maxY = height*cellSize;
      if (v->x > maxX) v->x = maxX-0.1f;
      if (v->y > maxY) v->y = maxY-0.1f;
    }

    //Returns true if the given circle touch any entity
    bool containsEntities (const Vector2& center, float radius, const Entity* source=NULL, int entityMask=0) const;
    
    //Return a list of entities that are touched by a circle.
    //A newly allocated list is returned, freeing it is the caller's resposibility
    list<Entity*>* entitiesIn (const Vector2& center, float radius, const Entity* source=NULL, int entityMask=0) const;

    //Returns if the given grid cell contains at least one entity of type. "source" isn't taken into account
    bool containsEntity (int x, int y, int entityMask, const Entity* source=NULL) const {
      const list<Entity*>& l = grid[x][y]->entities;
      if (l.empty()) { //early stopping saves some time because iterator creation is kind of costly
        return false;
      } else {
        ASSERT(inside(x,y));
        LIST_FOREACH_CONST(Entity*, l, iter) {
          if ((*iter != source) && ((*iter)->getType() & entityMask))
            return true;
        }
        return false;
      }
    }

    const Vector2& getOrigin () const { return origin; }

    void debugDraw () const;
  private:
    GridCell*** grid;
    const Vector2 origin;
    const unsigned width;
    const unsigned height;
    const float cellSize;

    //Outer invisible border (surrounding the grid, o is the grid origin)
    //  |-------------|
    //  |    b0       |
    //  |-o---------|-|
    //  |b|  grid   |b|
    //  |3|         |1|
    //  |-|---------|-|
    //  |  b2         |
    //  |-------------|
    //
    WallEntity* borders[4]; 

    GridCell* getCell (const Vector2& p) const {
      int x = getCellX(p);
      int y = getCellY(p);
      if (x == -1 || y == -1)
        return NULL;
      return grid[x][y];
    }

    bool collideBorders (const Vector2& startPos, const BoundingVolume* mover, const Vector2& move, CollisionResult* result) const;

    //Adds the cells touched by circle at position to touchedCells, using count as counter
    void touchCells (const BCircle* circle, const Vector2& position) const;
    void touchCells (const AABBox* bbox, const Vector2& position) const;

    //Returns the number of touched Cells. Cells can be fetched using touchedCells array
    void findTouchedCells (const Vector2& start, const Vector2& move) const;

    //FIXME: use a set instead ?
    //This array is used for temporary storage of touched cells found by touchCells & co.
    //GridCells should be added to it by using ONLY _addTouched
    vector<GridCell*>* tmpTouched;
    //for each cell, store if it has already been included in tmpTouched
    //this is basically only for fast lookup
    bool* _touchedCells; 

    //tmpTouched management :
    //check if the cell [x][y] is in the touched cells
    inline
    bool _touched (unsigned x, unsigned y) const {
/*      const size_t l = tmpTouched->length();
      for (unsigned i=0; i<l; i++)
        if (tmpTouched->get(i)->x == x && tmpTouched->get(i)->y == y)
          return true;
      return false;*/
      return _touchedCells[y*width+x];
    }

    inline
    void _clearTouched () const {
      memset(_touchedCells, 0, sizeof(bool)*width*height);
      tmpTouched->clear();
    }

    inline
    void _addTouched (GridCell* c) const {
      if (!_touched(c->x, c->y)) {
        _touchedCells[c->y*width+c->x] = true;
        tmpTouched->append(c);
      }
    }
};

#endif /* GRID_H_ */
