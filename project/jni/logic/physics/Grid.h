#ifndef GRID_H_
#define GRID_H_

#include "def.h"
#include "BCircle.h"
#include "CollisionResult.h"
#include "logic/Entity.h"
#include "AABBox.h"
#include "containers/list.h"

struct GridCell : public Entity {
  GridCell (const Vector2& worldPos, unsigned x, unsigned y) :
    Entity(new AABBox(TILE_SIZE, TILE_SIZE, this)),
    x(x),y(y),touched(false), solid(false) {
    this->setPosition(worldPos);
  }

  eEntityType getType () const {
    return ENTITY_WALL;
  }

  void explode () {}

  const unsigned x;
  const unsigned y;
  bool touched;
  bool solid;
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
          //We add tile_size/2 because aabbox position is centered
          grid[x][y] = new GridCell(gridToWorld(x,y)+Vector2(TILE_SIZE/2, TILE_SIZE/2), x,y);
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

    void setSolid (int x, int y, bool solid) {
      ASSERT(inside(x,y));
      grid[x][y]->solid = true;
    }

    void addEntity (Entity* e);

    //It is ASSUMED that entity position is the one obtained after the last call to moveEntity
    void removeEntity (Entity* e);

    void moveEntity (Entity* e, const Vector2& move);

    void clearTouched ();

    bool touched (unsigned x, unsigned y) const {
      if (!inside(x,y))
        return false;
      return grid[x][y]->touched;
    }

    bool trace (const Entity* mover, const Vector2& move, CollisionResult* result) const;

    unsigned getWidth () const {
      return width;
    }

    unsigned getHeight () const {
      return height;
    }

    //returns -1 if outside grid
    int getCellX (const Vector2& point) const {
      const int x = (int)((point.x-origin.x)/cellSize);
      if (x < 0 || x > (int)width)
        return -1;
      return x;
    }

    //returns -1 if outside grid
    int getCellY (const Vector2& point) const {
      const int y = (int)((point.y-origin.y)/cellSize);
      if (y < 0 || y > (int)height)
        return -1;
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

  private:
    GridCell*** grid;
    const Vector2 origin;
    const unsigned width;
    const unsigned height;
    const float cellSize;

    GridCell* getCell (const Vector2& p) const {
      int x = getCellX(p);
      int y = getCellY(p);
      if (x == -1 || y == -1)
        return NULL;
      return grid[x][y];
    }

    //Adds the cells touched by circle at position to touchedCells, using count as counter
    void touchCells (const BCircle* circle, const Vector2& position, unsigned* count) const;

    //This is a temporary array used to retrieve data from touchedCells.
    //We know that at most, a ray cast through our grid will touch sqrt(width^2+height^2)*2, so
    //this array is allocated once to contain that amount of data and reused then
    GridCell** touchedCells;

    //Returns the number of touched Cells. Cells can be fetched using touchedCells array
    unsigned findTouchedCells (const Vector2& start, const Vector2& move) const;
};

#endif /* GRID_H_ */
