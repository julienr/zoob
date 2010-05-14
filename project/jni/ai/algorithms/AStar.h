#ifndef ASTAR_H_
#define ASTAR_H_

#include "logic/physics/Grid.h"
#include "logic/Path.h"
#include "containers/binaryheap.h"
#include "logic/AbstractGrid.h"

struct AStarCell {
  int gCost; //cost from start to this cell
  int hCost; //ESTIMATE cost from this cell to end
  int getFCost () const { return gCost + hCost; } //total cost for path going through this cell

  AbstractGrid<AStarCell>::Cell* parent;
  bool closed;

  void reset () {
    gCost = MOOB_INF;
    hCost = MOOB_INF;
    parent = NULL;
    closed = false;
  }
};

/**
 * Implementation of the A-star algorithm to work on the game grid
 */
class AStar : public AbstractGrid<AStarCell> {
  public:
    AStar (const Grid& grid);

    //Returns a newly allocated (to be freed by caller) path representing the
    //shortest path between start and end. (Positions stored in path are in the middle of cells)
    //Returns NULL if no path can be found
    Path* shortestWay (const Vector2& start, const Vector2& end);
  protected:
    void _resetCells ();

    Path* reconstructPath (const Cell* c);

    //Returns true if the given cell is free from obstacle, false otherwise
    bool walkable (const Cell* c);

    static int neighDist (const Cell* c1, const Cell* c2);
    static int heuristicDist (const Cell* c1, const Cell* c2);

    struct cellFCostCompare {
        bool operator() (const Cell* c1, const Cell* c2) {
          return c1->data.getFCost() < c2->data.getFCost();
        }
    };

    binaryheap<Cell*, AStar::cellFCostCompare> openset;
};

#endif /* ASTAR_H_ */
