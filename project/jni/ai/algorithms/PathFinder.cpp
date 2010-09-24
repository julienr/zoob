#include "PathFinder.h"

PathFinder::PathFinder (const Grid& grid) : astar(grid) {

}

Path* PathFinder::findPath (const Vector2& start, const Vector2& end, Entity* e, StopCondition* cond) {
  return astar.shortestWay(start, end, e, cond);
}

bool PathFinder::needsReplanning (Entity* e) {
  return true;
}
