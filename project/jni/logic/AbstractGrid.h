#ifndef ABSTRACTGRID_H
#define ABSTRACTGRID_H

#include "logic/physics/Grid.h"

//An abstract class representing a copy of the logical grid in which cells can contain additionnal
//informations

//T is the type of data contained in one cell
template <class T>
class AbstractGrid {
  public:
    AbstractGrid (const Grid& grid)
      : grid(grid), gridW(grid.getWidth()), gridH(grid.getHeight()) {
      cells = new Cell**[gridW];
      for (int x=0; x<gridW; x++) {
        cells[x] = new Cell*[gridH];
        for (int y=0; y<gridH; y++)
          cells[x][y] = new Cell(x,y);
      }
    }

    ~AbstractGrid () {
      for (int x=0; x<gridW; x++) {
        for (int y=0; y<gridH; y++)
          delete cells[x][y];
        delete [] cells[x];
      }
      delete [] cells;
    }


    struct Cell {
      Cell(int x, int y) : x(x), y(y) {}
      const int x;
      const int y;
      T data;

      bool operator == (const Cell& other) const {
        return x == other.x && y == other.y;
      }
    };

    int getWidth () const {
      return gridW;
    }

    int getHeight () const {
      return gridH;
    }

    bool inside (int x, int y) const {
      return grid.inside(x,y);
    }

    float getCellSize () const {
      return grid.getCellSize();
    }

    //Return cell at (x,y) OR NULL if (x,y) is outside
    Cell* getCellAt (int x, int y) const {
      if (x<0 || y<0 ||
          x>=gridW || y>=gridH)
        return NULL;
      return cells[x][y];
    }

  protected:
    const Grid& grid;
    const int gridW;
    const int gridH;
    Cell*** cells;
};

#endif
