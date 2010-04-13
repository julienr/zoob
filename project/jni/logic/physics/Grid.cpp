#include "Grid.h"
#include "lib/Math.h"
#include "CollisionManager.h"

void Grid::clearTouched () {
  for (unsigned x=0; x<width; x++) {
    for (unsigned y=0; y<height; y++) {
      grid[x][y]->touched = false;
      for (list<Entity*>::iterator e=grid[x][y]->entities.begin(); e.hasNext(); e++)
        (*e)->collided = false;
    }
  }
}

void Grid::moveEntity (Entity* e, const Vector2& move) {
  ASSERT(e->getBVolume()->getType() == TYPE_CIRCLE);
  const BCircle* bcircle = static_cast<const BCircle *>(e->getBVolume());
  const Vector2& p = e->getPosition();
  const Vector2& end = p+move;

  //Remove from old cells
  for (list<GridCell*>::iterator i = e->touchedCells.begin(); i.hasNext(); i++)
    (*i)->entities.remove(e);
  e->touchedCells.clear();

  //Add to new
  unsigned numTouched = 0;
  touchCells(bcircle, end, &numTouched);
  for (unsigned i=0; i<numTouched; i++) {
    if (!touchedCells[i]->solid) {
      e->touchedCells.append(touchedCells[i]);
      touchedCells[i]->entities.append(e);
    }
  }
}

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

    touchedCells[count++] = grid[x][y];
    //We got into a solid cell, no need to go further => Fixme: sure ?
    /*if (grid[x][y]->solid)
      return count;*/

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
    touchedCells[count++] = grid[endX][endY];
  return count;
}

bool collideAgainstCell (GridCell* cell,
                            const Entity* sourceEntity,
                            const Vector2& startPos,
                            const BoundingVolume* mover,
                            const Vector2& move,
                            CollisionResult* result) {
  //Touched cell is solid, check collision against its aabb
  CollisionResult r;
  if (cell->solid) {
    if (CollisionManager::MovingAgainstStill(cell->getBVolume(), mover, move, &r)
         && r.tFirst < result->tFirst) {
      (*result) = r;
      result->collidedEntity = cell;
      result->colPoint = startPos+move*r.tFirst;
      return true;
    }
  } else {
    //Check collision against touchedCells entities list
    for (list<Entity*>::iterator iter = cell->entities.begin(); iter.hasNext(); iter++) {
      Entity* otherEnt = *iter;
      if (otherEnt == sourceEntity)
        continue;
      if (!otherEnt->isSolid())
        continue;

      const BoundingVolume* bvol = otherEnt->getBVolume();

      bool col = CollisionManager::MovingAgainstStill(bvol, mover, move, &r);
      if (col && (r.tFirst < result->tFirst)) {
        (*result) = r;
        result->collidedEntity = otherEnt;
        result->colPoint = startPos+move*r.tFirst;
        return true;
      }
    }
  }
  return false;
}

bool Grid::push(const Entity* mover, const Vector2& move, CollisionResult* result) const {
  ASSERT(mover->getBVolume()->getType() == TYPE_CIRCLE);
  const BCircle* circle = static_cast<const BCircle*>(mover->getBVolume());
  ASSERT(circle->getRadius()*2 < cellSize);
  ASSERT(move.length() < cellSize);
  //Calculate starting position covered cells
  // Since the circle diameter is < cellSize, this is simply the cells that contains the extreme points
  // of the circle on the axis
  unsigned numTouched = 0;
  touchCells(circle, circle->getPosition(), &numTouched);
  touchCells(circle, circle->getPosition()+move, &numTouched);

  CollisionResult r;
  result->tFirst = MOOB_INF;
  bool collided = false;
  for (unsigned i=0; i<numTouched; i++) {
    touchedCells[i]->touched = true;
    collided |= collideAgainstCell(touchedCells[i], mover, mover->getPosition(), mover->getBVolume(), move, result);
  }
  /*if (collided) {
    LOGE("Collision, tFirst : %f", r.tFirst);
  }*/
  return collided;
}

void Grid::addEntity (Entity* e) {
  const BoundingVolume* bvol = e->getBVolume();
  unsigned numTouched = 0;
  if (bvol->getType() == TYPE_CIRCLE)
    touchCells(static_cast<const BCircle*>(bvol), e->getPosition(), &numTouched);
  else if (bvol->getType() == TYPE_AABBOX)
    touchCells(static_cast<const AABBox*>(bvol), e->getPosition(), &numTouched);
  else
    ASSERT(false);

  if (numTouched == 0)
    LOGE("addEntity : numTouched = 0");
  e->touchedCells.clear();
  for (unsigned i=0; i<numTouched; i++) {
    touchedCells[i]->entities.append(e);
    e->touchedCells.append(touchedCells[i]);
  }
}

void Grid::removeEntity (Entity* e) {
  for (list<GridCell*>::iterator i = e->touchedCells.begin(); i.hasNext(); i++)
    (*i)->entities.remove(e);
  e->touchedCells.clear();
}

//Add cell to touchedCells and increment count if cond is true and (x,y) is inside grid
void Grid::addCellIf (unsigned x, unsigned y, bool cond, unsigned* count) const {
  if (cond && inside(x,y)) {
    /*Check if cell is already inside*/
    for (unsigned i=0; i<(*count); i++)
      if (touchedCells[i]->x == (x) && touchedCells[i]->y == (y))
        return;
    touchedCells[(*count)++] = grid[(x)][(y)];
  }
}

//FIXME: should check if a cell is already in our touched list before adding
void Grid::touchCells (const BCircle* circle, const Vector2& position, unsigned* count) const {
  ASSERT(circle->getRadius()*2 < cellSize);
  const float r = circle->getRadius();
  const int c[2] = {getCellX(position), getCellY(position)};
  //Calculate our position relative to the cell top-left corner
  const Vector2 tlPos = position - (Vector2(c[0], c[1])*cellSize + origin);
  //Now, if distance to one border is < r, we have to add this cell to the touched list
  if (inside(c[0], c[1]))
    touchedCells[(*count)++] = grid[c[0]][c[1]];
  addCellIf(c[0]-1, c[1], tlPos.x < r, count); //left cell
  addCellIf(c[0]+1, c[1], (cellSize-tlPos.x) < r, count); //right cell
  addCellIf(c[0], c[1]-1, tlPos.y < r, count); //top cell
  addCellIf(c[0], c[1]+1, (cellSize-tlPos.y) < r, count); //bottom cell
  addCellIf(c[0]-1, c[1]-1, (tlPos.x < r) && (tlPos.y < r), count); //top-left
  addCellIf(c[0]-1, c[1]+1, (tlPos.x < r) && (cellSize-tlPos.y < r), count); //bottom-left
  addCellIf(c[0]+1, c[1]-1, (cellSize-tlPos.x < r) && (tlPos.y < r), count); //top-right
  addCellIf(c[0]+1, c[1]+1, (cellSize-tlPos.x < r) && (cellSize-tlPos.y < r), count); //bottom-right
}

//FIXME: should check if a cell is already in our touched list before adding
void Grid::touchCells (const AABBox* bbox, const Vector2& position, unsigned* count) const {
  ASSERT(bbox->getHeight() <= cellSize && bbox->getWidth() <= cellSize);
  //Just check the 4 box's corners
  Vector2 c[4];
  bbox->getCorners(c);
  for (int i=0; i<4; i++) {
    GridCell* cell = getCell(c[i]);
    if (cell)
      touchedCells[(*count)++] = cell;
  }
}

/*bool Grid::traceRay (const Vector2& start, const Vector2& end, CollisionResult* result) const {
  unsigned numTouched = findTouchedCells(points[0], move);
  for (unsigned j=0; j<numTouched; j++) {
    touchedCells[j]->touched = true;
    if (touchedCells[j]->solid) {

    }
  }
}*/

/*
bool Grid::trace (const BCircle* circle, const Vector2& move, CollisionResult* result) const {
  ASSERT(circle->getRadius()*2 < cellSize);
  // We are tracing through the grid with a circle
  // We ENFORCE the circle's diameter to be less than cellSize, so we can simplify the collision
  // detections.
  // With these hypothesis, we can say that all the cells potentially touched by our circle during the trace
  // can be found by using two rays that represent the "extremities" of the circle on an axis perpendicular to the
  // trace direction.
  Vector2 perpAxis(move.y, -move.x);
  perpAxis.normalize();
  //the two points from which our rays will start
  const Vector2& cPos = circle->getPosition();
  Vector2 points[2];
  points[0] = cPos + perpAxis*circle->getRadius();
  points[1] = cPos - perpAxis*circle->getRadius();

  int minDist = MOOB_INF;

  const float moveLength = (cPos+move).length();

  for (int i=0; i<2; i++) {
    unsigned numTouched = findTouchedCells(points[0], move);
    for (unsigned j=0; j<numTouched; j++) {
      touchedCells[j]->touched = true;
      if (touchedCells[j]->solid) {
        const Vector2 col = gridToWorld(touchedCells[j]->x,touchedCells[j]->y) - move*circle->getRadius();
        const float dist = (col-cPos).length();
        if (dist < minDist) {
          minDist = dist;
          result->colPoint = col;
          result->tFirst = moveLength/(col - cPos).length();
          result->tLast = MOOB_INF;
          result->normal = -move;
        }
      }
    }
  }

  return (minDist < MOOB_INF);
}
*/
