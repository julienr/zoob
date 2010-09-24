#include "PathFinder.h"

PathFinder::PathFinder (const Grid& grid) : astar(grid, &reservationTable) {

}

Path* PathFinder::findPath (const Vector2& start, const Vector2& end, Entity* e, float entityMoveSpeed, StopCondition* cond) {
  return astar.shortestWay(start, end, e, entityMoveSpeed, cond);
}

bool PathFinder::needsReplanning (Entity* e) {
  return true;
}
