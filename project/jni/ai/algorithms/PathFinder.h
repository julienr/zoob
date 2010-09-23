#ifndef PATHFINDER_H_
#define PATHFINDER_H_

#include "logic/Path.h"
#include "logic/physics/Grid.h"
#include "AStar.h"

//Provides pathfinding services to game units
class PathFinder {
  public:
    //This can be used to early-stop path finding based on some condition (like check if the current cell is
    // visible, then no need to go further, just stop)
    struct StopCondition : public AStar::StopCondition {};

  public:
    PathFinder (const Grid& grid);

    //Finds a path from start to end for given entity. Checks cond at each cell if it is not NULL
    //Returns the found path or NULL if not such path exists
    Path* findPath (const Vector2& start, const Vector2& end, Entity* e, StopCondition* cond=NULL);

    //Returns true if the given Entity should replan its path (by calling findPath).
    bool needsReplanning (Entity* e);

    int getClearance (int x, int y) const {
      return astar.getClearance(x,y);
    }

  private:
    AStar astar;
};

#endif /* PATHFINDER_H_ */
