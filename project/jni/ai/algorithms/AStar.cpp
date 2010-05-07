#include "AStar.h"
#include "string.h"

Path* AStar::reconstructPath (const Cell* c) {
  //calculate number of nodes in path
  int numNodes = 0;
  const Cell* curr = c;
  while (curr->parent) {
    numNodes++;
    curr = curr->parent;
  }

  //FIXME: really ?
  if (numNodes == 0) {
    _resetCells();
    return NULL;
  }

  Vector2* nodes = new Vector2[numNodes];
  float cs2 = 0;//grid.getCellSize()/2.0f;

  curr = c;
  for (int i=numNodes-1; i>=0; i--) {
    nodes[i] = Vector2(curr->x+cs2, curr->y+cs2);
    curr = curr->parent;
  }

  _resetCells();
  return new Path(numNodes, nodes);
}

/**For all the distance calculation, we consider one cell to be 10 width. This means
 * that a horizontal or vertical move will cost 10. A diagonal move will cost sqrt(20) ~= 14.14 = 14
 */
int AStar::neighDist (const Cell* c1, const Cell* c2) {
  //should only be used for neighbouring cells
  ASSERT(abs(c1->x-c2->x) <= 1 && abs(c1->y-c2->y) <= 1);
  if ((*c1) == (*c2))
    return 0; //same cell
  else if ((c1->x == c2->x) || (c1->y == c2->y))
    return 10;//horiz/vert
  else
    return 14;//diagonal
}

//Return manhattan distance between c1 and c2
int AStar::heuristicDist (const Cell* c1, const Cell* c2) {
  return 10*(abs(c2->x-c1->x) + abs(c2->y-c1->y));
}

void AStar::_resetCells () {
  for (int x=0; x<gridW; x++) {
    for (int y=0; y<gridH; y++) {
      cells[x][y]->reset();
    }
  }
  openset.clear();
}

bool AStar::walkable (const Cell* c) {
  Grid::eCellContent content = grid.contentAt(c->x, c->y);
  //FIXME: we consider everything but walls walkable
  return content != Grid::WALL;
}

//#define ALOGE(...) LOGE(__VA_ARGS__)
#define ALOGE(...)
Path* AStar::shortestWay (const Vector2& startPos, const Vector2& endPos) {
  Cell* end = cells[grid.getCellXBounded(endPos.x)][grid.getCellYBounded(endPos.y)];
  Cell* start = cells[grid.getCellX(startPos.x)][grid.getCellY(startPos.y)];


  ALOGE("\n\n---- AStar from (%i,%i) to (%i,%i)", start->x, start->y, end->x, end->y);


  start->gCost = 0;
  start->hCost = heuristicDist(start, end);
  openset.insert(start);

  while (openset.size() > 0) {
    //current is the cell with lowest F cost
    Cell* current = openset.removeRoot();
    if (*current == *end)
      return reconstructPath(current);
    ALOGE("current (%i,%i) g=%u, h=%u", current->x, current->y, current->gCost, current->hCost);
    current->closed = true;
    //For all 8 neighbours
    for (int nx=-1; nx<=1; nx++) {
      for (int ny=-1; ny<=1; ny++) {
        const int x = current->x+nx;
        const int y = current->y+ny;
        if ((x == current->x && y == current->y) ||
            x < 0 || y < 0 || x >= gridW || y >= gridH)
          continue;

        Cell* neigh = cells[x][y];
        ALOGE("\tneigh (%i,%i) : walkable : %i, content : %i", neigh->x, neigh->y,
               walkable(neigh),
               grid.contentAt(neigh->x, neigh->y));

        /** There is a special case here. If neigh is end, we have to add it to openset
         * because we'll often run our algorithms and target an unreacheable cell (containing another tank)
         */
        if (!(*neigh == *end) && (neigh->closed || !walkable(neigh))) {
          ALOGE("\t\tunwalkable : closed %i", neigh->closed);
          continue;
        }

        const int tentativeGCost = current->gCost + neighDist(current, neigh);
        ALOGE("\t\ttentative g=%u", tentativeGCost);

        if (!openset.contains(neigh)) {
          ALOGE("\t\tinserting new");
          neigh->parent = current;
          neigh->gCost = tentativeGCost;
          neigh->hCost = heuristicDist(neigh, end);
          openset.insert(neigh);
        } else if (tentativeGCost < neigh->gCost) {
          ALOGE("\t\tbetter than existing");
          neigh->parent = current;
          neigh->gCost = tentativeGCost;
          //h cost stay the same
        } else
          ALOGE("\t\tworst than existing");
      }
    }
  }
  LOGE("NO path found");
  _resetCells();
  return NULL;
}

