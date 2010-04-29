#include "AStar.h"
#include "string.h"

Path* AStar::reconstructPath (const Cell* c) {
  const int numNodes = c->gCost/10;
  LOGE("numNodes : %i", numNodes);
  if (numNodes <= 0) {
    _clean();
    return NULL;
  }

  Vector2* nodes = new Vector2[numNodes];
  float cs2 = grid.getCellSize()/2.0f;

  const Cell* curr = c;
  for (int i=numNodes-1; i>=0; i--) {
    nodes[i] = Vector2(curr->x+cs2, curr->y+cs2);
    curr = curr->parent;
  }

  _clean();
  return new Path(numNodes, nodes);
}

/**For all the distance calculation, we consider one cell to be 10 width. This means
 * that a horizontal or vertical move will cost 10. A diagonal move will cost sqrt(20) ~= 14.14 = 14
 */
int AStar::Cell::neighDist (const Cell* c1, const Cell* c2) {
  //should only be used for neighbouring cells
  ASSERT(abs(c1->x-c2->x) <= 1 && abs(c1->y-c2->y) <= 1);
  if (c1 == c2)
    return 0; //same cell
  else if ((c1->x == c2->x) || (c1->y == c2->y))
    return 10;//horiz/vert
  else
    return 14;//diagonal
}

//Return manhattan distance between c1 and c2
int AStar::Cell::heuristicDist (const Cell* c1, const Cell* c2) {
  return 10*(abs(c2->x-c1->x) + abs(c2->y-c1->y));
}

void AStar::_clean () {
  //Have to do the memset by row, because the first dimension is pointer to array
  for (int x=0; x<gridW; x++) {
    memset(closed[x], 0, sizeof(bool)*gridH);
  }
  openset.clear();
  LIST_FOREACH(Cell*, cells, c) {
    delete *c;
  }
  cells.clear();
}

Path* AStar::shortestWay (const Vector2& startPos, const Vector2& endPos) {
  Cell* start = new Cell((uint8_t)grid.getCellX(startPos), (uint8_t)grid.getCellY(startPos));
  Cell* end = new Cell((uint8_t)grid.getCellX(endPos), (uint8_t)grid.getCellY(endPos));
  cells.append(start);
  cells.append(end);
  LOGE("\n\n---- AStar from (%i,%i) to (%i,%i)", start->x, start->y, end->x, end->y);

  start->gCost = 0;
  start->hCost = Cell::heuristicDist(start, end);
  openset.insert(start);

  while (openset.size() > 0) {
    //current is the cell with lowest F cost
    Cell* current = openset.removeRoot();
    if ((*current) == (*end))
      return reconstructPath(current);
    LOGE("current (%i,%i) g=%u, h=%u", current->x, current->y, current->gCost, current->hCost);
    closed[current->x][current->y] = true;
    //insertInClosedset(current);
    //For all neighbours of lowestF
    for (int nx=-1; nx<=1; nx++) {
      for (int ny=-1; ny<=1; ny++) {
        const int x = current->x+nx;
        const int y = current->y+ny;
        if ((x == current->x && y == current->y) ||
            x < 0 || y < 0 || x >= gridW || y >= gridH)
          continue;

        Cell* neigh = new Cell((uint8_t)x,(uint8_t)y);
        cells.append(neigh);
        if (closed[neigh->x][neigh->y])
          continue;

        const int tentativeGCost = current->gCost + Cell::neighDist(current, neigh);
        LOGE("\tneigh (%i,%i) tentative g=%u", neigh->x, neigh->y, tentativeGCost);

        Cell** existing = openset.find(neigh);
        if (!existing) {
          LOGE("\t\tinserting new");
          neigh->parent = current;
          neigh->gCost = tentativeGCost;
          neigh->hCost = Cell::heuristicDist(neigh, end);
          openset.insert(neigh);
        } else if (tentativeGCost < (*existing)->gCost) {
          LOGE("\t\tbetter than existing");
          delete neigh;
          Cell* c = *existing;
          c->parent = current;
          c->gCost = tentativeGCost;
          c->hCost = Cell::heuristicDist(c, end);
        } else
          LOGE("\t\tworst than existing");

      }
    }
  }
  LOGE("NO path found");
  _clean();
  return NULL;
}

