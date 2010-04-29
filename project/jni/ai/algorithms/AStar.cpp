#include "AStar.h"
#include "string.h"

Path* AStar::reconstructPath (const Cell& c) {
  const int numNodes = c.getFCost();
  LOGE("numNodes : %i", numNodes);
  if (numNodes <= 0)
    return NULL;

  Vector2* nodes = new Vector2[numNodes];
  float cs2 = grid.getCellSize()/2.0f;

  uint8_t px = c.x;
  uint8_t py = c.y;
  nodes[numNodes-1] = Vector2(c.x+cs2, c.y+cs2);

  for (int i=numNodes-2; i>=0; i--) {
    LOGE("(%i,%i)", px, py);
    const uint8_t* p = parent[px][py];
    px = p[0];
    py = p[1];
    nodes[i] = Vector2(px+cs2, py+cs2);
  }

  return new Path(numNodes, nodes);
}

//Return manhattan distance between c1 and c2
uint8_t AStar::Cell::dist (const Cell& c1, const Cell& c2) {
  return abs(c2.x-c1.x) + abs(c2.y-c1.y);
}

void AStar::_reinit () {
  //Have to do the memset by row, because the first dimension is pointer to array
  for (int x=0; x<gridW; x++) {
    memset(closed[x], 0, sizeof(bool)*gridH);
    memset(open[x], 0, sizeof(bool)*gridH);
  }
  openset.clear();
}

Path* AStar::shortestWay (const Vector2& startPos, const Vector2& endPos) {
  _reinit();
  Cell start((uint8_t)grid.getCellX(startPos), (uint8_t)grid.getCellY(startPos));
  Cell end((uint8_t)grid.getCellX(endPos), (uint8_t)grid.getCellY(endPos));
  LOGE("AStar from (%i,%i) to (%i,%i)", start.x, start.y, end.x, end.y);

  openset.insert(start);
  start.gCost = 0;
  start.hCost = Cell::dist(start, end);

  while (openset.size() > 0) {
    //current is the cell with lowest F cost
    Cell current = removeFromOpenset();
    if (current == end)
      return reconstructPath(current);
    LOGE("current (%i,%i) g=%u, h=%u", current.x, current.y, current.gCost, current.hCost);
    insertInClosedset(current);
    //For all neighbours of lowestF
    for (int nx=-1; nx<=1; nx++) {
      for (int ny=-1; ny<=1; ny++) {
        const int x = current.x+nx;
        const int y = current.y+ny;
        if ((x == current.x && y == current.y) ||
            x < 0 || y < 0 || x >= gridW || y >= gridH)
          continue;

        Cell neigh((uint8_t)x,(uint8_t)y);
        if (isInClosedset(neigh))
          continue;

        const uint8_t tentativeGCost = current.gCost + Cell::dist(current, neigh);
        LOGE("\tneigh (%i,%i) tentative g=%u", neigh.x, neigh.y, tentativeGCost);

        bool tentative_is_better = false;
        if (!isInOpenset(neigh)) {
          insertInOpenset(neigh);
          tentative_is_better = true;
        } else if (tentativeGCost < neigh.gCost)
          tentative_is_better = true;

        if (tentative_is_better) {
          parent[neigh.x][neigh.y][0] = current.x;
          parent[neigh.x][neigh.y][1] = current.y;
          neigh.gCost = tentativeGCost;
          neigh.hCost = Cell::dist(neigh, end);
        }
      }
    }
  }
  LOGE("NO path found");
  return NULL;
}

