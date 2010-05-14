#include "VisibilityGrid.h"
#include "containers/binaryheap.h"
#include "logic/ShadowPolygon.h"
#include "logic/Game.h"

VisibilityGrid::VisibilityGrid(const Grid& grid)
  : AbstractGrid<VisCell>(grid), djikstraStart(NULL), djikstraSource(NULL) {
  _resetCells();
}

void VisibilityGrid::calculateVisibility (const Game* game) {
  const vector<ShadowPolygon*>& shadows = game->getPlayerShadows();

  //To speed up things a little, we use circle to approximate our cells
  const float r = grid.getCellSize()/2.0f;

  //FIXME: this doesn't work completely for the cells behind the intersection of two obstacles
  for (int x = 0; x < gridW; x++) {
    for (int y = 0; y < gridH; y++) {
      cells[x][y]->data.visibility = VISIBLE;
      for (unsigned i = 0; i < shadows.length(); i++) {
        eRelativePos relPos = shadows[i]->classifyCircle(grid.gridToWorld(x,y), r);
        eVisibility& cellVis = cells[x][y]->data.visibility;
        if (relPos == INSIDE)
          cellVis = HIDDEN;
        else if (relPos == INTERSECT && cellVis == VISIBLE)
          cellVis = PENUMBRA;
        else if (relPos == OUTSIDE && cellVis == VISIBLE)
          cellVis = VISIBLE;
        /*if (shadows[i]->fullyInside(grid.gridToWorld(x, y), r))
          cells[x][y]->data.visible = false;
        else
          cells[x][y]->data.visible &= true;*/
      }
    }
  }
}

void VisibilityGrid::_resetCells () {
  for (int x=0; x<gridW; x++) {
    for (int y=0; y<gridH; y++) {
      cells[x][y]->data.reset(grid.gridToWorld(x,y));
    }
  }
}

void VisibilityGrid::print () const {
  printf("\nvisibility...\n");
  for (int y=0; y<gridH; y++) {
    for (int x=0; x<gridW; x++) {
      if (!walkable(cells[x][y]))
        printf("# ");
        //printf("#      ");
      else
        //printf("%i[%3i] ", cells[x][y]->data.visible, cells[x][y]->data.dist);
        printf("%i ", cells[x][y]->data.visibility);
    }
    printf("\n");
  }
}

Path* VisibilityGrid::pathTo (const Cell* dest) const {
  //calculate number of nodes in path
  int numNodes = 0;
  const Cell* curr = dest;
  while (curr->data.parent) {
    numNodes++;
    curr = curr->data.parent;
  }

  if (numNodes == 0)
    return NULL;

  Vector2* nodes = new Vector2[numNodes];

  curr = dest;
  for (int i = numNodes - 1; i >= 0; i--) {
    //nodes[i] = grid.gridToWorld(curr->x, curr->y);
    nodes[i] = curr->data.waypoint;
    curr = curr->data.parent;
  }

  return new Path(numNodes, nodes);
}

Path* VisibilityGrid::pathToCenterBiggestHidden () const {
  vector<Cell*>* biggestGroup = NULL;
  Vector2 biggestCenter; //center (Average of points) of the biggest group

  bool visited[gridW][gridH];
  for (int x=0; x<gridW; x++)
    for (int y=0; y<gridH; y++)
      visited[x][y] = false;

  //Determine all the adjacent hidden groups by flood fill
  int coords[2];
  while (true) {
    //Find next hidden cell
    coords[0] = coords[1] = -1;
    for (int x=0; x<gridW; x++) {
      for (int y=0; y<gridH; y++) {
        if (!visited[x][y] && walkable(cells[x][y]) && (cells[x][y]->data.visibility == HIDDEN)) {
          coords[0] = x;
          coords[1] = y;
          break;
        }
      }
    }

    if (coords[0] == -1)
      break;

    //Flood fill
    vector<Cell*>* currentGroup = new vector<Cell*>(10);
    Vector2 center(0,0);
    list<Cell*> openlist;
    openlist.append(cells[coords[0]][coords[1]]);
    visited[coords[0]][coords[1]] = true;
    while (!openlist.empty()) {
      Cell* current = openlist.firstElement();
      openlist.removeFirst();
      currentGroup->append(current);

      for (int nx=-1; nx<=1; nx++) {
        for (int ny=-1; ny<=1; ny++) {
          const int x = current->x+nx;
          const int y = current->y+ny;
          if ((x == current->x && y == current->y) ||
              x < 0 || y < 0 || x >= gridW || y >= gridH)
            continue;
          if (visited[x][y])
            continue;
          if (!walkable(cells[x][y]))
            continue;
          if (cells[x][y]->data.visibility != HIDDEN)
            continue;
          visited[x][y] = true;
          openlist.append(cells[x][y]);
          center += cells[x][y]->data.waypoint;
        }
      }
    }
    ASSERT(visited[coords[0]][coords[1]]);

    //Replace previous biggest group if we're bigger
    if (!biggestGroup || currentGroup->length() > biggestGroup->length()) {
      delete biggestGroup;
      biggestCenter = center/(float)currentGroup->length();
      biggestGroup = currentGroup;
    } else
      delete currentGroup;
  }

  if (!biggestGroup) {
    LOGE("no hidden groups");
    return NULL;
  }

  //LOGE("biggest size : %i, center(%f,%f)" , biggestGroup->length(), biggestCenter.x, biggestCenter.y);
  int minIdx;
  float minDist = MOOB_INF;
  for (size_t i=0; i<biggestGroup->length(); i++) {
    float d = (biggestCenter-biggestGroup->get(i)->data.waypoint).length();
    if (d < minDist) {
      minDist = d;
      minIdx = i;
    }
  }
  return pathTo(biggestGroup->get(minIdx));
}

Path* VisibilityGrid::pathToClosest (bool vis) const {
  int coords[2] = {-1,-1};
  int closest = MOOB_INF;

  for (int x=0; x<gridW; x++) {
    for (int y=0; y<gridH; y++) {
      if ((cells[x][y]->data.visibility == vis) && cells[x][y]->data.dist < closest) {
        coords[0] = x;
        coords[1] = y;
        closest = cells[x][y]->data.dist;
      }
    }
  }

  if (coords[0] == -1) {
    LOGE("[pathToClosest] No path to cell");
    return NULL;
  }

  //LOGE("start[%i,%i], closest[%i,%i]", djikstraStart->x, djikstraStart->y, coords[0], coords[1]);
  return pathTo(cells[coords[0]][coords[1]]);
}


/**For all the distance calculation, we consider one cell to be 10 width. This means
 * that a horizontal or vertical move will cost 10. A diagonal move will cost sqrt(20) ~= 14.14 = 14
 */
int VisibilityGrid::neighDist (const Cell* c1, const Cell* c2) {
  //should only be used for neighbouring cells
  ASSERT(abs(c1->x-c2->x) <= 1 && abs(c1->y-c2->y) <= 1);

  int d;
  if ((*c1) == (*c2))
    d = 0; //same cell
  else if ((c1->x == c2->x) || (c1->y == c2->y))
    d = 10;//horiz/vert
  else
    d = 14;//diagonal

  //Penalty if the cell is visible
  if (c1->data.visibility != HIDDEN)
    d *= 2;
  return d;
}

bool VisibilityGrid::walkable (const Cell* c) const {
  return !grid.containsEntity(c->x, c->y, ENTITY_WALL | ENTITY_ROCKET | ENTITY_TANK, djikstraSource);
}

struct cellDistCompare {
  bool operator () (const AbstractGrid<VisCell>::Cell* c1,
                      const AbstractGrid<VisCell>::Cell* c2) {
    return c1->data.dist < c2->data.dist;
  }
};

//Adapt the waypoints for the neighbours of c
void VisibilityGrid::_adaptWaypoint (const Cell* c, float unitSize) {
  const float hcs = getCellSize()/2;
  for (int nx=-1; nx<=1; nx++) {
    for (int ny=-1; ny<=1; ny++) {
      const int x = c->x+nx;
      const int y = c->y+ny;
      if ((x == c->x && y == c->y) ||
          x < 0 || y < 0 || x >= gridW || y >= gridH)
        continue;

      Cell* neigh = cells[x][y];

      if (neigh->data.closed || !walkable(neigh))
        continue;

      const Vector2 dir = Vector2(nx,ny).getNormalized();
      const Vector2 neighVertex = c->data.waypoint + Vector2(nx*hcs, ny*hcs);

      const float dist = (neigh->data.waypoint-neighVertex).length();
      neigh->data.waypoint += dir*(unitSize-dist);
    }
  }
}

/**
 * This function push the waypoints of cells neighbouring an obstacle so a unit with a bcircle
 * of unitSize radius will follow a smooth path and won't end up colliding with the obstacle
 */
void VisibilityGrid::_adaptWaypoints (float unitSize) {
  for (int x=0; x<gridW; x++) {
    for (int y=0; y<gridH; y++) {
      const Cell* c = cells[x][y];
      if (!walkable(c))
        _adaptWaypoint(c, unitSize);
    }
  }
}

void VisibilityGrid::djikstra (const Vector2& startPos, const Entity* source) {
  _resetCells();
  if (source) {
    ASSERT(source->getBVolume()->getType() == TYPE_CIRCLE);
    _adaptWaypoints(source->getBVolume()->getWidth()/2.0f);
  }
  djikstraSource = source;
  djikstraStart = cells[grid.getCellX(startPos.x)][grid.getCellY(startPos.y)];
  binaryheap<Cell*, cellDistCompare> openset(gridW*gridH);

  djikstraStart->data.dist = 0;

  //Insert all cells in open set
  for (int x=0; x<gridW; x++)
    for (int y=0; y<gridH; y++)
      openset.insert(cells[x][y]);

  while (openset.size() > 0) {
    //LOGE("openset size : %i", openset.size());
    Cell* current = openset.removeRoot();
    if (current->data.dist == INT_INF) { //all remaining verts unreachable
      //LOGE("all remaining verts unreachable");
      return;
    }

    //LOGE("current dist : %i", current->data.dist);

    //For all 8 neighbours
    for (int nx=-1; nx<=1; nx++) {
      for (int ny=-1; ny<=1; ny++) {
        const int x = current->x+nx;
        const int y = current->y+ny;
        if ((x == current->x && y == current->y) ||
            x < 0 || y < 0 || x >= gridW || y >= gridH)
          continue;

        Cell* neigh = cells[x][y];

        if (neigh->data.closed || !walkable(neigh))
          continue;

        int alt = current->data.dist + neighDist(current, neigh);
        if (alt < neigh->data.dist) {
          neigh->data.dist = alt;
          neigh->data.parent = current;
          openset.updateElement(neigh);
        }
      }
    }
    current->data.closed = true;
  }
}

