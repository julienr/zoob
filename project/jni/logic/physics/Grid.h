#ifndef GRID_H_
#define GRID_H_

#include "def.h"
#include "BCircle.h"
#include "CollisionResult.h"

struct GridCell {
  GridCell () : touched(false) {}
  bool touched;
};

class Grid {
  public:
    //We have to subdivise the area [width, height] in cells of size cellSize
    //Origin is the origin of the coordinate systems wolrd objects to be placed in the grid are in
    Grid (const Vector2& origin, unsigned width, unsigned height, float cellSize) :
      origin(origin), width(width), height(height), cellSize(cellSize) {
      grid = new GridCell*[width];
      for (unsigned x=0; x<width; x++)
        grid[x] = new GridCell[height];
      const unsigned diagLength = ceilf(sqrtf(width*width+height*height));
      //FIXME: do we really need *2  ? I think for a ray, max number is diagLength
      LOGE("Max number of touched cells : %u", diagLength*2);
      touchedCells = new GridCell*[diagLength*2];
    }

    ~Grid () {
      for (unsigned x=0; x<width; x++)
          delete [] grid[x];
      delete [] grid;
      delete [] touchedCells;
    }

    void clearTouched () {
      for (unsigned x=0; x<width; x++)
        for (unsigned y=0; y<height; y++)
          grid[x][y].touched = false;
    }

    bool touched (unsigned x, unsigned y) const {
      if (!inside(x,y))
        return false;
      return grid[x][y].touched;
    }

    bool trace (const BCircle* circle, const Vector2& move, CollisionResult* result) const;

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

  private:
    GridCell** grid;
    const Vector2 origin;
    const unsigned width;
    const unsigned height;
    const float cellSize;

    //This is a temporary array used to retrieve data from touchedCells.
    //We know that at most, a ray cast through our grid will touch sqrt(width^2+height^2)*2, so
    //this array is allocated once to contain that amount of data and reused then
    GridCell** touchedCells;

    //Returns the number of touched Cells. Cells can be fetched using touchedCells array
    unsigned findTouchedCells (const Vector2& start, const Vector2& move) const;
};

#endif /* GRID_H_ */
