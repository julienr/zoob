#include "AStar.h"
#include "string.h"

AStar::AStar (const Grid& grid) 
  : AbstractGrid<AStarCell>(grid), openset(10) {
  _resetCells();
}

Path* AStar::reconstructPath (const Cell* c) {
  //calculate number of nodes in pathentity
  int numNodes = 0;
  const Cell* curr = c;
  while (curr->data.parent) {
    numNodes++;
    curr = curr->data.parent;
  }

  if (numNodes == 0) {
    //no path found (or already at dest)
    _resetCells();
    return NULL;
  }

  Vector2* nodes = new Vector2[numNodes];

  curr = c;
  for (int i=numNodes-1; i>=0; i--) {
    nodes[i] = grid.gridToWorld(curr->x, curr->y);
    curr = curr->data.parent;
  }

  _resetCells();
  return new Path(numNodes, nodes);
}

//See http://harablog.wordpress.com/2009/01/29/clearance-based-pathfinding/
//For each cell, clearance gives the maximum width (or height) of a square that would
//extend from the cell to the down-right direction. This indicates how big a unit can
//be and still be able to pass through this cell
//
//We require an entity parameter because we tank entities into accounts and therefore,
//we don't want our own entity to block us
void AStar::calculateClearance (Entity* entity) {
  const int maxSquare = MIN(gridW, gridH);

  //By starting with lower-right cell (which will either have clearance of 1 if walkable or -1 otherwise), we first
  //scan by line and then move the the previous line
  //a cell clearance is the min of the clearance of the adjacent cells plus one
  for (int y=gridH-1; y>=0; y--) {
    for (int x=gridW-1; x>=0; x--) {
      if (isSolid(cells[x][y], entity)) {
        cells[x][y]->data.clearance = -1;
      } else {
        Cell* adj1 = getCellAt(x+1,y);
        Cell* adj2 = getCellAt(x, y+1);
        Cell* adj3 = getCellAt(x+1, y+1);
        if (adj1 && adj2 && adj3) {
          int min = adj1->data.clearance;
          min = MIN(min, adj2->data.clearance);
          min = MIN(min, adj3->data.clearance);
          if (min == -1)
            cells[x][y]->data.clearance = 1;
          else
            cells[x][y]->data.clearance = min + 1;
        } else { //this is a border cell => clearance = 1
          cells[x][y]->data.clearance = 1;
        }
      }
    }
  }
}

/**For all the distance calculation, we consider one cell to be 10 width. This means
 * that a horizontal or vertical move will cost 10. A diagonal move will cost sqrt(20) ~= 14.14 = 14
 */
int AStar::neighDist (const Cell* c1, const Cell* c2, int entitySize) {
  //should only be used for neighbouring cells
  ASSERT(abs(c1->x-c2->x) <= 1 && abs(c1->y-c2->y) <= 1);
  if ((*c1) == (*c2))
    return 0; //same cell
  else if ((c1->x == c2->x) || (c1->y == c2->y))
    return 10;//horiz/vert
  else {
    //return 14;
    //Diagonal moves are forbidden...
    LOGE("AStar::neightDist : Trying to move in diagonal from (%i,%i) to (%i,%i)", c1->x, c1->y, c2->x, c2->y);
    return 100000;
  }
}

//Return manhattan distance between c1 and c2
int AStar::heuristicDist (const Cell* c1, const Cell* c2) {
  return 10*(abs(c2->x-c1->x) + abs(c2->y-c1->y));
}

void AStar::_resetCells () {
  for (int x=0; x<gridW; x++) {
    for (int y=0; y<gridH; y++) {
      cells[x][y]->data.reset();
    }
  }
  openset.clear();
}

bool AStar::isSolid (const Cell* c, Entity* entity) {
  //FIXME: should consider tanks as solid
  return grid.containsEntity(c->x, c->y, ENTITY_WALL | ENTITY_ROCKET | ENTITY_TANK, entity);
}

bool AStar::walkable (const Cell* c, int entitySize, Entity* entity) {
  return !isSolid(c, entity) && c->data.clearance >= entitySize;
}

//#define ALOGE(...) LOGE(__VA_ARGS__)
#define ALOGE(...)
Path* AStar::shortestWay (const Vector2& startPos, const Vector2& endPos, Entity* e) {
  //Calculate source size
  int entitySize = 1;
  if (e != NULL) {
    ASSERT(e->getBVolume()->getType() == TYPE_CIRCLE);
    const BCircle* circle = static_cast<const BCircle*>(e->getBVolume());
    entitySize = (int)Math::ceil(2*circle->getRadius()/grid.getCellSize());
  }

  Cell* end = cells[grid.getCellXBounded(endPos.x)][grid.getCellYBounded(endPos.y)];
  Cell* start = cells[grid.getCellX(startPos.x)][grid.getCellY(startPos.y)];

  ALOGE("\n\n---- AStar from (%i,%i) to (%i,%i)", start->x, start->y, end->x, end->y);

  calculateClearance(e);

  start->data.gCost = 0;
  start->data.hCost = heuristicDist(start, end);
  openset.insert(start);

  while (openset.size() > 0) {
    //current is the cell with lowest F cost
    Cell* current = openset.removeRoot();
    if (*current == *end)
      return reconstructPath(current);
    ALOGE("current (%i,%i) g=%u, h=%u", current->x, current->y, current->data.gCost, current->data.hCost);
    current->data.closed = true;
    //For all 8 neighbours
    for (int nx=-1; nx<=1; nx++) {
      for (int ny=-1; ny<=1; ny++) {
        const int x = current->x+nx;
        const int y = current->y+ny;
        if ((x == current->x && y == current->y) ||
            x < 0 || y < 0 || x >= gridW || y >= gridH)
          continue;

        if (Math::abs(nx) == Math::abs(ny)) //no diagonal moves
          continue;

        Cell* neigh = cells[x][y];
        ALOGE("\tneigh (%i,%i) : walkable : %i, content : %i", neigh->x, neigh->y,
               walkable(neigh, entitySize),
               grid.contentAt(neigh->x, neigh->y));

        /** There is a special case here. If neigh is end, we have to add it to openset
         * because we'll often run our algorithms and target an unreacheable cell (containing another tank)
         */
        if (!(*neigh == *end) && (neigh->data.closed || !walkable(neigh, entitySize, e))) {
          ALOGE("\t\tunwalkable : closed %i", neigh->closed);
          continue;
        }

        const int tentativeGCost = current->data.gCost + neighDist(current, neigh, entitySize);
        ALOGE("\t\ttentative g=%u", tentativeGCost);

        if (!openset.contains(neigh)) {
          ALOGE("\t\tinserting new");
          neigh->data.parent = current;
          neigh->data.gCost = tentativeGCost;
          neigh->data.hCost = heuristicDist(neigh, end);
          openset.insert(neigh);
        } else if (tentativeGCost < neigh->data.gCost) {
          ALOGE("\t\tbetter than existing");
          neigh->data.parent = current;
          neigh->data.gCost = tentativeGCost;
          openset.updateElement(neigh);
          //h cost stay the same
        } else {
          ALOGE("\t\tworst than existing");
        }
      }
    }
  }
  LOGE("NO path found");
  _resetCells();
  return NULL;
}

