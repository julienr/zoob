#include "VisibilityGrid.h"
#include "containers/binaryheap.h"
#include "logic/ShadowPolygon.h"
#include "logic/Game.h"

VisibilityGrid::VisibilityGrid(const Grid& grid)
  : AbstractGrid<VisCell>(grid) {
}

void VisibilityGrid::calculateVisibility (Game* game) {
  const vector<ShadowPolygon*>& shadows = game->getPlayerShadows();

  //To speed up things a little, we use circle to approximate our cells
  const float r = grid.getCellSize()/2.0f;

  for (int x = 0; x < gridW; x++) {
    for (int y = 0; y < gridH; y++) {
      cells[x][y]->data.visibility = VISIBLE;

      for (unsigned i = 0; i < shadows.length(); i++) {
        eRelativePos relPos = shadows[i]->classifyCircle(grid.gridToWorld(x,y), r);
        eVisibility& cellVis = cells[x][y]->data.visibility;
        if (relPos == INSIDE) {
          cellVis = HIDDEN;
          //can BREAK here because there is no need to test for further
          //inclusion in shadows since it's already hidden
          break;
        } else if (relPos == INTERSECT && cellVis == VISIBLE)
          cellVis = PENUMBRA;
        else if (relPos == OUTSIDE && cellVis == VISIBLE)
          cellVis = VISIBLE;
      }
    }
  }
}

bool VisibilityGrid::findCenterBiggestHidden (const Entity* source, int& outX, int& outY) const {
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
        if (!visited[x][y] && walkable(cells[x][y], source) && (cells[x][y]->data.visibility == HIDDEN)) {
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
          if (!walkable(cells[x][y], source))
            continue;
          if (cells[x][y]->data.visibility != HIDDEN)
            continue;
          visited[x][y] = true;
          openlist.append(cells[x][y]);
          center += grid.gridToWorld(x,y);
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
    return false;
  }

  //LOGE("biggest size : %i, center(%f,%f)" , biggestGroup->length(), biggestCenter.x, biggestCenter.y);
  //Look for cell closest to center in the biggest group
  int minIdx=0;
  float minDist = MOOB_INF;
  for (size_t i=0; i<biggestGroup->length(); i++) {
    Cell* _c = biggestGroup->get(i);
    float d = (biggestCenter-grid.gridToWorld(_c->x, _c->y)).length();
    if (d < minDist) {
      minDist = d;
      minIdx = i;
    }
  }
  Cell* closest = biggestGroup->get(minIdx);
  delete biggestGroup;
  outX = closest->x;
  outY = closest->y;
  return true;
}

bool VisibilityGrid::walkable (const Cell* c, const Entity* entity) const {
  return !grid.containsEntity(c->x, c->y, ENTITY_WALL | ENTITY_ROCKET | ENTITY_TANK, entity);
}

