#include "Grid.h"
#include "lib/Math.h"
#include "CollisionManager.h"
#include "BLine.h"
#include "lib/Utils.h"
#include "logic/Level.h"

//We have to subdivise the area [width, height] in cells of size cellSize
//Origin is the origin of the coordinate systems wolrd objects to be placed in the grid are in
Grid::Grid (const Vector2& origin, unsigned w, unsigned h, float cellSize) 
  : origin(origin), width(w/cellSize), height(h/cellSize), cellSize(cellSize) {
  grid = new GridCell**[width];
  for (unsigned x=0; x<width; x++) {
    grid[x] = new GridCell*[height];
    for (unsigned y=0; y<height; y++) {
      grid[x][y] = new GridCell(x,y);
    }
  }
  tmpTouched = new vector<GridCell*>(20);
  _touchedCells = new bool[width*height];

  borders[0] = new WallEntity(w, 1, Vector2(origin.x+w/2.0f, origin.y-cellSize));
  borders[1] = new WallEntity(1, h, Vector2(origin.x+w+cellSize, origin.y+h/2.0f));
  borders[2] = new WallEntity(w, 1, Vector2(origin.x+w/2.0f, origin.y+h+cellSize));
  borders[3] = new WallEntity(1, h, Vector2(origin.x-cellSize, origin.y+h/2.0f));
}

Grid::~Grid () {
  for (unsigned x=0; x<width; x++) {
    for (unsigned y=0; y<height; y++)
      delete grid[x][y];
    delete [] grid[x];
  }
  delete [] _touchedCells;
  delete [] grid;
  delete tmpTouched;

  for (int i=0; i<4; i++)
    delete borders[i];
}



void Grid::dbg_clearTouched () {
  for (unsigned x=0; x<width; x++) {
    for (unsigned y=0; y<height; y++) {
      grid[x][y]->dbgTouched = false;
    }
  }
}

void Grid::unmarkCollided () {
  for (unsigned x=0; x<width; x++) {
    for (unsigned y=0; y<height; y++) {
      for (list<Entity*>::iterator e=grid[x][y]->entities.begin(); e.hasNext(); e++)
        (*e)->collided = false;
    }
  }
}

void Grid::entityMoved (Entity* e) {
  ASSERT(e->getBVolume()->getType() == TYPE_CIRCLE);
  const BCircle* bcircle = static_cast<const BCircle *>(e->getBVolume());

  //Remove from old cells
  for (list<GridCell*>::iterator i = e->touchedCells.begin(); i.hasNext(); i++)
    (*i)->entities.remove(e);
  e->touchedCells.clear();

  //Add to new
  _clearTouched(); 
  touchCells(bcircle, e->getPosition());
  for (unsigned i=0; i<tmpTouched->length(); i++) {
    e->touchedCells.append(tmpTouched->get(i));
    tmpTouched->get(i)->entities.append(e);
  }
}

void Grid::findTouchedCells (const Vector2& start, const Vector2& move) const {
  if (Math::epsilonEq(move.x, 0) && Math::epsilonEq(move.y, 0))
    return;

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
    return;
  }

  //LOGE("[findTouchedCells] starting at (%i,%i)", x, y);

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
    tDeltaX = cellSize/fabs(nm.x);
  }

  if (Math::epsilonEq(nm.y, 0)) {
    tMaxY = MOOB_INF;
    tDeltaY = MOOB_INF;
  } else {
    if (stepY > 0) tMaxY = (cellSize - cy)/nm.y;
    else tMaxY = -cy/nm.y;
    tDeltaY = cellSize/fabs(nm.y);
  }
  //LOGE("(tMaxX,tMaxY) : (%f,%f)\t(tDeltaX,tDeltaY) : (%f,%f)", tMaxX, tMaxY, tDeltaX, tDeltaY);

  while (true) {
    //If endX,endY is in the boundaries, we must have gone past it to stop, otherwise, we stop
    //just before leaving the boundaries
    if (inside(endX, endY) && ((stepX > 0 && x >= endX) || (stepX <= 0 && x <= endX)) &&
        ((stepY > 0 && y >= endY) || (stepY <= 0 && y <= endY)))
        break;
    else if (!inside(x,y))
      break;

    _addTouched(grid[x][y]);

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
    _addTouched(grid[endX][endY]);
}

bool Grid::collideBorders (const Vector2& startPos, const BoundingVolume* mover, const Vector2& move, CollisionResult* result) const {
  bool collided = false;
  for (int i=0; i<4; i++) {
    CollisionResult r;
    bool col = CollisionManager::MovingAgainstStill(borders[i]->getPosition(), borders[i]->getBVolume(), startPos, mover, move, &r);
    if (col && (r.tFirst < result->tFirst)) {
      (*result) = r;
      result->collidedEntity = borders[i];
      result->colPoint = startPos+move*r.tFirst;
      collided = true;
    }
  }
  return collided;
}


//entityMask determine which entities WON'T be collided against
//(0~ENTITY_ROCKET) won't collide against rocket for example
bool collideAgainstCell (GridCell* cell,
                         const Entity* sourceEntity,
                         const Vector2& startPos,
                         const BoundingVolume* mover,
                         const Vector2& move,
                         CollisionResult* result,
                         int entityMask=0) {
  CollisionResult r;

  bool collided = false;

  //Check collision against cell entities list
  for (list<Entity*>::iterator iter = cell->entities.begin(); iter.hasNext(); iter++) {
    Entity* otherEnt = *iter;
    if (otherEnt == sourceEntity)
      continue;
    if (!otherEnt->isSolid())
      continue;
    if (otherEnt->getType() & entityMask)
      continue;

    const BoundingVolume* bvol = otherEnt->getBVolume();

    bool col = CollisionManager::MovingAgainstStill(otherEnt->getPosition(), bvol, startPos, mover, move, &r);
    if (col && (r.tFirst < result->tFirst)) {
      (*result) = r;
      result->collidedEntity = otherEnt;
      result->colPoint = startPos+move*r.tFirst;
      collided = true;
    }
  }
  return collided;
}

bool Grid::push(const Entity* mover, const Vector2& move, CollisionResult* result, int entityMask) const {
  ASSERT(mover->getBVolume()->getType() == TYPE_CIRCLE);
  const BCircle* circle = static_cast<const BCircle*>(mover->getBVolume());

  // To calculate cells covered by movement, we decompose the move in small moves whose length
  // don't exceed cellSize. We just merge the set of cells touched by each of these small moves
  _clearTouched();
  // Start position
  touchCells(circle, mover->getPosition());
  if (move.length() < cellSize) {
    //Simplest case
    touchCells(circle, mover->getPosition()+move);
  } else {
    //What we are doing here is dividing the big move in small moves
    //that are no longer than cellSize (cellMove) and we
    const Vector2 cellMove = move.getNormalized()*cellSize;
    Vector2 cumulativeMove;
    float div = move.length()/cellMove.length();
    int num = floor(div);
    for (int i=0; i<num; i++) {
      cumulativeMove += cellMove;
      touchCells(circle, mover->getPosition()+cumulativeMove);
    }
    touchCells(circle, mover->getPosition()+move);
  }

  result->tFirst = MOOB_INF;
  bool collided = false;
  for (unsigned i=0; i<tmpTouched->length(); i++) {
    tmpTouched->get(i)->dbgTouched = true;
    collided |= collideAgainstCell(tmpTouched->get(i),
                                   mover,
                                   mover->getPosition(),
                                   mover->getBVolume(),
                                   move,
                                   result,
                                   entityMask);
  }
  collided |= collideBorders(mover->getPosition(), mover->getBVolume(), move, result);
  return collided;
}

void Grid::addEntity (Entity* e) {
  const BoundingVolume* bvol = e->getBVolume();
  _clearTouched();
  if (bvol->getType() == TYPE_CIRCLE)
    touchCells(static_cast<const BCircle*>(bvol), e->getPosition());
  else if (bvol->getType() == TYPE_AABBOX)
    touchCells(static_cast<const AABBox*>(bvol), e->getPosition());
  else
    ASSERT(false);

  if (tmpTouched->length() == 0)
    LOGE("addEntity : numTouched = 0");
  //LOGE("addEntity: numTouched : %u", tmpTouched->length());
  e->touchedCells.clear();
  for (unsigned i=0; i<tmpTouched->length(); i++) {
    tmpTouched->get(i)->entities.append(e);
    e->touchedCells.append(tmpTouched->get(i));
  }
}

void Grid::removeEntity (Entity* e) {
  for (list<GridCell*>::iterator i = e->touchedCells.begin(); i.hasNext(); i++)
    (*i)->entities.remove(e);
  e->touchedCells.clear();
}

bool Grid::containsEntities (const Vector2& center, float radius, const Entity* source, int entityMask) const {
  BCircle circ(radius);
  _clearTouched();
  touchCells(&circ, center);
  for (unsigned i=0; i<tmpTouched->length(); i++) {
    for (list<Entity*>::iterator iter = tmpTouched->get(i)->entities.begin(); iter.hasNext(); iter++) {
      Entity* entity = *iter;
      if (entity == source)
        continue;
      if (entity->getType() & entityMask)
        continue;

      const BoundingVolume* bv = entity->getBVolume();
      if (bv->getType() == TYPE_AABBOX && CollisionManager::AABBIntersectCircle(entity->getPosition(), static_cast<const AABBox*>(bv),center, &circ)) {
        return true;
      } else if (bv->getType() == TYPE_CIRCLE && CollisionManager::CircleIntersectCircle(entity->getPosition(), static_cast<const BCircle*>(bv),center, &circ)) {
        return true;
      }
    }
  }
  return false;
}

list<Entity*>* Grid::entitiesIn (const Vector2& center, float radius, const Entity* source, int entityMask) const {
  list<Entity*>* touchedList = new list<Entity*>();
  BCircle circ(radius);
  _clearTouched();
  touchCells(&circ, center);
  for (unsigned i=0; i<tmpTouched->length(); i++) {
    for (list<Entity*>::iterator iter = tmpTouched->get(i)->entities.begin(); iter.hasNext(); iter++) {
       Entity* entity = *iter;
       if (entity == source)
         continue;
       if (entity->getType() & entityMask)
         continue;

       const BoundingVolume* bv = entity->getBVolume();
       if (bv->getType() == TYPE_AABBOX && CollisionManager::AABBIntersectCircle(entity->getPosition(), static_cast<const AABBox*>(bv),
                                                                                 center, &circ)) {
         if (!touchedList->contains(entity))
           touchedList->append(entity);
       } else if (bv->getType() == TYPE_CIRCLE && CollisionManager::CircleIntersectCircle(entity->getPosition(), static_cast<const BCircle*>(bv),
                                                                                         center, &circ)) {
         if (!touchedList->contains(entity))
           touchedList->append(entity);
       }
    }
  }
  return touchedList;
}

void Grid::touchCells (const BCircle* circle, const Vector2& position) const {
  const float r = circle->getRadius();
  //All the touched cells are contained between the two extremities of this circle on the two axis
  const int startX = getCellXBounded(position.x-r);
  const int endX = getCellXBounded(position.x+r);
  const int startY = getCellYBounded(position.y-r);
  const int endY = getCellYBounded(position.y+r);
  for (int x=startX; x<=endX; x++) {
    for (int y=startY; y<=endY; y++) {
      _addTouched(grid[x][y]);
    }
  }
}

void Grid::touchCells (const AABBox* bbox, const Vector2& position) const {
  //All the touched cells are contained between the two extremities of this box on the two axis
  const float hw = bbox->getWidth()/2.0f;
  const float hh = bbox->getHeight()/2.0f;
  const int startX = getCellXBounded(position.x-hw);
  const int endX = getCellXBounded(position.x+hw);
  const int startY = getCellYBounded(position.y-hh);
  const int endY = getCellYBounded(position.y+hh);
  for (int x=startX; x<=endX; x++) {
    for (int y=startY; y<=endY; y++) {
      _addTouched(grid[x][y]);
    }
  }
}

bool Grid::traceRay (const Entity* source, const Vector2& start, const Vector2& move, CollisionResult* result, int entityMask) const {
  result->tFirst = MOOB_INF;
  bool collided = false;
  _clearTouched();
  findTouchedCells(start, move);
  BLine line;
  for (unsigned i=0; i<tmpTouched->length(); i++) {
    tmpTouched->get(i)->dbgTouched = true;
    collided |= collideAgainstCell(tmpTouched->get(i),
                                   source,
                                   start,
                                   &line,
                                   move,
                                   result,
                                   entityMask);
  }
  collided |= collideBorders (start, source->getBVolume(), move, result);
  return collided;
}

#undef DEBUG_TRACE
#ifdef DEBUG_TRACE_CIRCLE
vector<Vector2> debugPoints(2);
vector<Vector2> debugMoves(2);
#endif

bool Grid::traceCircle (Entity* source, const Vector2& start, const Vector2& move, float radius, CollisionResult* result, int entityMask) const {
  ASSERT(2*radius < cellSize);
  // We are tracing through the grid with a circle
  // We ENFORCE the circle's diameter to be less than cellSize, so we can simplify the collision
  // detections.
  // With these hypothesis, we can say that all the cells potentially touched by our circle during the trace
  // can be found by using two rays that represent the "extremities" of the circle on an axis perpendicular to the
  // trace direction.
  Vector2 perpAxis(move.y, -move.x);
  perpAxis.normalize();
  //the two points from which our rays will start
  Vector2 points[2];
  points[0] = start + perpAxis*radius;
  points[1] = start - perpAxis*radius;

  result->tFirst = MOOB_INF;
  BCircle circle(radius);
  _clearTouched();
  for (int i = 0; i<2; i++) {
    //LOGE("points[%i] (%f,%f)", i, points[i].x, points[i].y);
    findTouchedCells(points[i], move);
#ifdef DEBUG_TRACE_CIRCLE
    debugPoints.append(points[i]);
    debugMoves.append(move);
#endif
  }

  bool collided = false;
  for (unsigned j = 0; j < tmpTouched->length(); j++) {
    tmpTouched->get(j)->dbgTouched = true;
    collided |= collideAgainstCell(tmpTouched->get(j), source, start, &circle,
                                   move, result, entityMask);
  }
  collided |= collideBorders(start, source->getBVolume(), move, result);
  return collided;
}

#include "view/GameView.h"

void Grid::debugDraw () const {
#ifdef DEBUG_TRACE_CIRCLE
  glPointSize(3.0f);
  glColor4f(1,1,1,1);
  glBegin(GL_POINTS);
  for (int i=0; i<debugPoints.length(); i++) {
    glVertex3f(debugPoints[i].x, debugPoints[i].y, 0);
  }
  glEnd();
  glPointSize(1.0f);
  glBegin(GL_LINES);
  for (int i=0; i<debugPoints.length(); i++) {
    const Vector2& end = debugPoints[i]+debugMoves[i];
    glVertex3f(debugPoints[i].x, debugPoints[i].y, 0);
    glVertex3f(end.x, end.y, 0);
  }
  glEnd();
  debugMoves.clear();
  debugPoints.clear();
#endif
  GLW::colorWhite();
  for (int i=0; i<4; i++)
    GameView::drawAABBox(static_cast<const AABBox*>(borders[i]->getBVolume()), borders[i]->getPosition());
}
