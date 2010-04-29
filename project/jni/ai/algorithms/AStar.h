#ifndef ASTAR_H_
#define ASTAR_H_

#include "logic/physics/Grid.h"
#include "logic/Path.h"

/**
 * Implementation of the A-star algorithm to work on the game grid
 */
class AStar {
  public:
    AStar (Grid* grid)
      : grid(grid) {
      ASSERT(grid != NULL);
    }

    //Returns a newly allocated (to be freed by caller) path representing the
    //shortest path between from and to. (Positions stored in path are in the middle of cells)
    //Returns NULL if no path can be found
    Path* shortestWay (const Vector2& from, const Vector2& to);
  protected:
    const Grid* grid;
};

#endif /* ASTAR_H_ */
