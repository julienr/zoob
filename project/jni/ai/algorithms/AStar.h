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
      closed = new bool*[gridW];
      open = new bool*[gridW];
      parent = new uint8_t**[gridW];
      for (int x=0; x<gridW; x++) {
        closed[x] = new bool[gridH];
        open[x] = new bool[gridH];
        parent[x] = new uint8_t*[gridH];
        for (int y=0; y<gridH ;y++)
          parent[x][y] = new uint8_t[2];
      }

    }

    ~AStar () {
      for (int x=0; x<gridW; x++) {
        delete [] open[x];
        delete [] closed[x];
        for (int y=0; y<gridH; y++)
          delete [] parent[x][y];
        delete [] parent[x];
      }
      delete [] open;
      delete [] closed;
      delete [] parent;
    }

    //Returns a newly allocated (to be freed by caller) path representing the
    //shortest path between start and end. (Positions stored in path are in the middle of cells)
    //Returns NULL if no path can be found
    Path* shortestWay (const Vector2& start, const Vector2& end);
  protected:
    void _reinit ();


    struct Cell {
        Cell (uint8_t x, uint8_t y) : x(x), y(y), gCost(0), hCost(0) {}
        uint8_t x;
        uint8_t y;
        uint8_t gCost; //cost from start to this cell
        uint8_t hCost; //ESTIMATE cost from this cell to end
        uint8_t getFCost () const { return gCost + hCost; } //total cost for path going through this cell

        bool operator == (const Cell& other) const {
          return x == other.x && y == other.y;
        }

        static uint8_t dist (const Cell& c1, const Cell& c2);
    };

    struct cellFCostCompare {
        bool operator() (const Cell& c1, const Cell& c2) {
          return c1.getFCost() < c2.getFCost();
        }
    };

    void insertInOpenset (const Cell& c) {
      openset.insert(c);
      open[c.x][c.y] = true;
    }

    bool isInOpenset (const Cell& c) {
      return open[c.x][c.y];
    }

    Cell removeFromOpenset () {
      Cell c = openset.removeRoot();
      open[c.x][c.y] = false;
      return c;
    }

    bool isInClosedset (const Cell& c) {
      return closed[c.x][c.y];
    }

    void insertInClosedset (const Cell& c) {
      closed[c.x][c.y] = true;
    }

    Path* reconstructPath (const Cell& c);

    const Grid& grid;
    const int gridW;
    const int gridH;

    //at parent[x][y] is the x([0]) and y([1]) coord of the parent of this cell
    uint8_t*** parent;

    bool** closed; //[grid.getWidth()][grid.getHeight()]
    /** We use two datastructures to manage openset. For openset inclusion/exclusion, we use a bitmap
     * the size of the grid. And for openset ordering with regards to the F cost, we use a binaryheap.
     * Of course, these two datastructures should always be coherent
     */
    bool** open; //[grid.getWidth()][grid.getHeight()]
    binaryheap<Cell, AStar::cellFCostCompare> openset;
};

#endif /* ASTAR_H_ */
