#ifndef GRID_H_
#define GRID_H_

#include "def.h"
#include "BCircle.h"
#include "CollisionResult.h"
#include "logic/Entity.h"
#include "AABBox.h"
#include "containers/list.h"

struct GridCell {
  GridCell (unsigned x, unsigned y) :
    x(x),y(y),touched(false){
  }

  const unsigned x;
  const unsigned y;
  bool touched;
  list<Entity*> entities;
};

class Grid {
  public:
    //We have to subdivise the area [width, height] in cells of size cellSize
    //Origin is the origin of the coordinate systems wolrd objects to be placed in the grid are in
    Grid (const Vector2& origin, unsigned width, unsigned height, float cellSize) :
      origin(origin), width(width), height(height), cellSize(cellSize) {
      grid = new GridCell**[width];
      for (unsigned x=0; x<width; x++) {
        grid[x] = new GridCell*[height];
        for (unsigned y=0; y<height; y++) {
          grid[x][y] = new GridCell(x,y);
        }
      }
      const unsigned diagLength = ceilf(sqrtf(width*width+height*height));
      //FIXME: do we really need *2  ? I think for a ray, max number is diagLength
      LOGE("Max number of touched cells : %u", diagLength*2);
      touchedCells = new GridCell*[diagLength*2];
    }

    ~Grid () {
      for (unsigned x=0; x<width; x++) {
        for (unsigned y=0; y<height; y++)
          delete grid[x][y];
          delete [] grid[x];
      }
      delete [] grid;
      delete [] touchedCells;
    }

    void addEntity (Entity* e);
    //@see CollisionManager::addWallFromPosition
    void addWallFromPosition (Entity* e);

    //It is ASSUMED that entity position is the one obtained after the last call to moveEntity
    void removeEntity (Entity* e);

    void moveEntity (Entity* e, const Vector2& move);

    void clearTouched ();

    bool touched (unsigned x, unsigned y) const {
      if (!inside(x,y))
        return false;
      return grid[x][y]->touched;
    }

    //To be used for movement < cellSize
    bool push (const Entity* mover, const Vector2& move, CollisionResult* result, int entityMask) const;

    bool traceRay (const Entity* source, const Vector2& start, const Vector2& move, CollisionResult* result, int entityMask) const;
    bool traceCircle (Entity* source, const Vector2& start, const Vector2& move, float radius, CollisionResult* result, int entityMask) const;
    bool trace (const BCircle* circle, const Vector2& move, CollisionResult* result, int entityMask) const;

    unsigned getWidth () const {
      return width;
    }

    unsigned getHeight () const {
      return height;
    }

    //This is mainly used for pathfinding
    enum eCellContent {
        EMPTY, //this cell is empty
        WALL, //this cell contains at least a wall (and possibly some other entities)
        ENTITIES //this cell contains NO wall but some other entities
    };

    //Return eCellContent describing the content of grid[x][y]
    //if not NULL, source won't be taken into account
    eCellContent contentAt (unsigned x, unsigned y, Entity* source=NULL) const {
      if (!inside(x,y))
        return WALL;
      if (grid[x][y]->entities.empty())
        return EMPTY;

      bool empty = source != NULL; //if entities only contains source, it will be considered empty
      LIST_FOREACH(Entity*, grid[x][y]->entities, iter) {
        if ((*iter)->getType() == ENTITY_WALL)
          return WALL;
        if ((*iter) != source)
          empty = false;
      }
      return empty?EMPTY:ENTITIES;
    }

    //returns -1 if outside grid
    int getCellX (const Vector2& point) const {
      return getCellX(point.x);
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

    Vector2 gridToWorld (int x, int y) const {
      return Vector2(x*cellSize + origin.x, y*cellSize + origin.y);
    }

    void capToGrid (Vector2* v) const {
      if (v->x < 0) v->x = 0.1f;
      if (v->y < 0) v->y = 0.1f;
      const float maxX = width*cellSize;
      const float maxY = height*cellSize;
      if (v->x > maxX) v->x = maxX-0.1f;
      if (v->y > maxY) v->y = maxY-0.1f;
    }

    //Return a list of entities that are touched by a circle.
    //A newly allocated list is returned, freeing it is the caller's resposibility
    list<Entity*>* entitiesIn (const Vector2& center, float radius) const;

  private:
    GridCell*** grid;
    const Vector2 origin;
    const unsigned width;
    const unsigned height;
    const float cellSize;

    //check if the cell [x][y] is in the touched cells
    bool _touched (unsigned x, unsigned y, unsigned count) const;

    GridCell* getCell (const Vector2& p) const {
      int x = getCellX(p);
      int y = getCellY(p);
      if (x == -1 || y == -1)
        return NULL;
      return grid[x][y];
    }

    //Adds the cells touched by circle at position to touchedCells, using count as counter
    void touchCells (const BCircle* circle, const Vector2& position, unsigned* count) const;
    void touchCells (const AABBox* bbox, const Vector2& position, unsigned* count) const;

    void addCellIf (unsigned x, unsigned y, bool cond, unsigned* count) const;

    //This is a temporary array used to retrieve data from touchedCells.
    //We know that at most, a ray cast through our grid will touch sqrt(width^2+height^2)*2, so
    //this array is allocated once to contain that amount of data and reused then
    GridCell** touchedCells;

    //Returns the number of touched Cells. Cells can be fetched using touchedCells array
    unsigned findTouchedCells (const Vector2& start, const Vector2& move) const;
};

#endif /* GRID_H_ */
