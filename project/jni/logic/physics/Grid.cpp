#include "Grid.h"
#include "lib/Math.h"

unsigned Grid::findTouchedCells (const Vector2& start, const Vector2& move) const {
  if (Math::epsilonEq(move.x, 0) && Math::epsilonEq(move.y, 0))
    return false;

  const Vector2 end = start+move;
  //see "A Fast Voxel Traversal Algorithm for Ray Tracing"
  //http://www.cse.yorku.ca/~amana/research/grid.pdf
  const int endX = getCellX(end);
  const int endY = getCellY(end);
  int x = getCellX(start);
  int y = getCellY(start);

  //The case were start is outside is not handled by cx/cy below
  if (x == -1 || y == -1) {
    LOGE("(%i,%i) outside grid", x, y);
    return 0;
  }


  const int stepX = Math::signum(move.x);
  const int stepY = Math::signum(move.y);
  //LOGE("(x,y) : (%i,%i)\t(stepX,stepY) : (%i,%i)\t(endX,endY) : (%i,%i)", x, y, stepX, stepY, endX, endY);

  //start pos relative to tile
  const float cx = start.x-(x*cellSize + origin.x);
  const float cy = start.y-(y*cellSize + origin.y);
  float tMaxX, tMaxY;
  float tDeltaX, tDeltaY;
  const Vector2 nm = move.getNormalized();
  if (Math::epsilonEq(nm.x, 0)) {
    tMaxX = MOOB_INF;
    tDeltaX = MOOB_INF;
  } else {
    if (stepX > 0) tMaxX = (cellSize - cx)/nm.x;
    else tMaxX = -cx/nm.x;
    tDeltaX = 1/fabs(nm.x);
  }

  if (Math::epsilonEq(nm.y, 0)) {
    tMaxY = MOOB_INF;
    tDeltaY = MOOB_INF;
  } else {
    if (stepY > 0) tMaxY = (cellSize - cy)/nm.y;
    else tMaxY = -cy/nm.y;
    tDeltaY = 1/fabs(nm.y);
  }
  //LOGE("(tMaxX,tMaxY) : (%f,%f)\t(tDeltaX,tDeltaY) : (%f,%f)", tMaxX, tMaxY, tDeltaX, tDeltaY);

  unsigned count = 0;
  while (true) {
    //If endX,endY is in the boundaries, we must have gone past it to stop, otherwise, we stop
    //just before leaving the boundaries
    if (inside(endX, endY) && ((stepX > 0 && x >= endX) || (stepX <= 0 && x <= endX)) &&
        ((stepY > 0 && y >= endY) || (stepY <= 0 && y <= endY)))
        break;
    else if (!inside(x,y))
      break;

    touchedCells[count++] = &grid[x][y];
    //LOGE("loop (x,y) : (%i,%i), (endX, endY) : (%i,%i)", x, y, endX, endY);
    if (tMaxX < tMaxY) {
      //LOGE("Moving on x");
      tMaxX += tDeltaX;
      x += stepX;
    } else {
      //LOGE("Moving on y");
      tMaxY += tDeltaY;
      y += stepY;
    }
  }
  //We'll of course touch the end cell (if it's in the boundary), so add it
  if (inside(endX,endY))
    touchedCells[count++] = &grid[endX][endY];
  //LOGE("count : %i", count);
  return count;
}

bool Grid::trace (const BCircle* circle, const Vector2& move, CollisionResult* result) const {
  ASSERT(circle->getRadius()*2 < cellSize);
  /** We are tracing through the grid with a circle
   * We ENFORCE the circle's diameter to be less than cellSize, so we can simplify the collision
   * detections.
   * With these hypothesis, we can say that all the cells potentially touched by our circle during the trace
   * can be found by using two rays that represent the "extremities" of the circle on an axis perpendicular to the
   * trace direction.
   */
  Vector2 perpAxis(move.y, move.x);
  perpAxis.normalize();
  //the two points from which our rays will start
  /*const Vector2 topPoint = circle->getPosition() + perpAxis*circle->getRadius();
  const Vector2 botPoint = circle->getPosition() - perpAxis*circle->getRadius();
  LOGE("topPoint (%f,%f)", topPoint.x, topPoint.y);*/

  unsigned numTouched = findTouchedCells(circle->getPosition(), move);
  for (unsigned i=0; i<numTouched; i++)
    touchedCells[i]->touched = true;

  return false;
}

