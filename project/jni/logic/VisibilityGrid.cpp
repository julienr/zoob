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

    for (int x = 0; x < gridW; x++) {
    for (int y = 0; y < gridH; y++) {
      cells[x][y]->data.visible = true;
      for (unsigned i = 0; i < shadows.length(); i++) {
        if (shadows[i]->fullyInside(grid.gridToWorld(x, y), r))
          cells[x][y]->data.visible = false;
        else
          cells[x][y]->data.visible &= true;
      }
    }
  }
}

void VisibilityGrid::_resetCells () {
  for (int x=0; x<gridW; x++) {
    for (int y=0; y<gridH; y++) {
      cells[x][y]->data.reset();
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
        printf("%i ", cells[x][y]->data.visible);
    }
    printf("\n");
  }
}

Path* VisibilityGrid::pathToClosestHidden () const {
  int coords[2] = {-1,-1};
  int closest = MOOB_INF;

  for (int x=0; x<gridW; x++) {
    for (int y=0; y<gridH; y++) {
      if (!cells[x][y]->data.visible && cells[x][y]->data.dist < closest) {
        coords[0] = x;
        coords[1] = y;
        closest = cells[x][y]->data.dist;
      }
    }
  }

  if (coords[0] == -1) {
    LOGE("No path to hidden cell");
    return NULL;
  }

  LOGE("start[%i,%i], closest[%i,%i]", djikstraStart->x, djikstraStart->y, coords[0], coords[1]);

  //calculate number of nodes in path
  int numNodes = 0;
  const Cell* curr = cells[coords[0]][coords[1]];
  while (curr->data.parent) {
    numNodes++;
    curr = curr->data.parent;
  }

  if (numNodes == 0)
    return NULL;

  Vector2* nodes = new Vector2[numNodes];

  curr = cells[coords[0]][coords[1]];
  for (int i = numNodes - 1; i >= 0; i--) {
    nodes[i] = grid.gridToWorld(curr->x, curr->y);
    curr = curr->data.parent;
  }

  return new Path(numNodes, nodes);
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
  if (c1->data.visible)
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

void VisibilityGrid::djikstra (const Vector2& startPos, const Entity* source) {
  _resetCells();
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

