#ifndef ASTAR_H_
#define ASTAR_H_

#include "logic/physics/Grid.h"
#include "logic/Path.h"
#include "containers/binaryheap.h"


/**
 * Implementation of the A-star algorithm to work on the game grid
 */
class AStar {
  public:
    AStar (const Grid& grid)
      : grid(grid), gridW(grid.getWidth()), gridH(grid.getHeight()), openset(10) {
      cells = new Cell**[gridW];
      for (int x=0; x<gridW; x++) {
        cells[x] = new Cell*[gridH];
        for (int y=0; y<gridH; y++)
          cells[x][y] = new Cell(x,y);
      }
      _resetCells();
    }

    ~AStar () {
      for (int x=0; x<gridW; x++) {
        for (int y=0; y<gridH; y++)
          delete cells[x][y];
        delete [] cells[x];
      }
      delete [] cells;
    }

    //Returns a newly allocated (to be freed by caller) path representing the
    //shortest path between start and end. (Positions stored in path are in the middle of cells)
    //Returns NULL if no path can be found
    Path* shortestWay (const Vector2& start, const Vector2& end);
  protected:
    void _resetCells ();

    struct Cell {
        Cell (int x, int y) : x(x), y(y) {}
        int gCost; //cost from start to this cell
        int hCost; //ESTIMATE cost from this cell to end
        int getFCost () const { return gCost + hCost; } //total cost for path going through this cell

        Cell* parent;
        bool closed;

        const int x;
        const int y;

        void reset () {
          gCost = MOOB_INF;
          hCost = MOOB_INF;
          parent = NULL;
          closed = false;
        }

        bool operator == (const Cell& other) const {
          return x == other.x && y == other.y;
        }
    };

    Path* reconstructPath (const Cell* c);

    //Returns true if the given cell is free from obstacle, false otherwise
    bool walkable (const Cell* c);

    const Grid& grid;
    const int gridW;
    const int gridH;
    Cell*** cells;

    static int neighDist (const Cell* c1, const Cell* c2);
    static int heuristicDist (const Cell* c1, const Cell* c2);

    struct cellFCostCompare {
        bool operator() (const Cell* c1, const Cell* c2) {
          return c1->getFCost() < c2->getFCost();
        }
    };

    binaryheap<Cell*, AStar::cellFCostCompare> openset;
};

#endif /* ASTAR_H_ */
