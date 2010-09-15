#include "VisibilityGrid.h"
#include "containers/binaryheap.h"
#include "logic/ShadowPolygon.h"
#include "logic/Game.h"

VisibilityGrid::VisibilityGrid(const Grid& grid)
  : AbstractGrid<VisCell>(grid) {
}

struct EdgeInfo {
  int ymin,ymax;
  //y=m*x+h edge equation
  float m;
  float b;

  EdgeInfo(const int x1, const int y1, const int x2, const int y2) {
    if (y2<y1) {
      ymin = y2;
      ymax = y1;
      m = (y1-y2)/(float)(x1-x2);
    } else {
      ymin = y1;
      ymax = y2;
      m = (y2-y1)/(float)(x2-x1);
    }
    b = y1-m*x1;
    //LOGE("b(1)=%f\tb(2)=%f", y1-m*x1, y2-m*x2);
    ASSERT(Math::epsilonEq(y1-m*x1, y2-m*x2));
  }
};

struct IntCompare {
  bool operator() (int i1, int i2) {
    return i1 < i2;
  }
};

void VisibilityGrid::calculateVisibility (Game* game) {
  //This is polygon filling, so we use the scanline rendering algorithm
  const vector<ShadowPolygon*>& shadows = game->getPlayerShadows();
  const Grid& grid = game->getColManager().getGrid();

  //FIXME: we could include this in the algo
  for (int x = 0; x < gridW; x++) {
      for (int y = 0; y < gridH; y++) {
        cells[x][y]->data.visibility = VISIBLE;
      }
  }

  Line levelClip[4];
  const unsigned levelW = game->getLevel()->getWidth();
  const unsigned levelH = game->getLevel()->getHeight();
  //top
  levelClip[0] = Line::fromPointAndNormal(Vector2(0,0), Vector2(0, -1));
  //bottom
  levelClip[2] = Line::fromPointAndNormal(Vector2(0, levelH), Vector2(0, 1));
  //right
  levelClip[1] = Line::fromPointAndNormal(Vector2(levelW, 0), Vector2(1, 0));
  //left
  levelClip[3] = Line::fromPointAndNormal(Vector2(0,0), Vector2(-1, 0));

  //First, create and fill edge table. Contains all the edges from all the polygons
  //and it is sorted from by ymin
  list<EdgeInfo>* edgeTable = new list<EdgeInfo>[gridH];

  LOGE("--calculateVisibility");
  LOGE("gridH : %i, gridW : %i", gridH, gridW);

  int numEdges = 0;

  int minY = gridH;
  int maxY = 0;
  for (unsigned i = 0; i < shadows.length(); i++) {
    const ShadowPolygon* sp = shadows[i];
    //clip the polygon to the level enclosing quad
    const Polygon pol = sp->toPolygon().clip(levelClip, 4);
    game->dbg_addDebugPolygon(DebugPolygon(pol, WHITE));

    const size_t num = pol.getNumVerts();
    for (size_t j=0; j<num; j++) {
      const Vector2& v1 = pol.getVerts()[j];
      const Vector2& v2 = pol.getVerts()[(j+1)%num];
      const int x1 = grid.getCellX(v1);
      const int y1 = grid.getCellY(v1);
      const int x2 = grid.getCellX(v2);
      const int y2 = grid.getCellY(v2);
      LOGE("verts (%f,%f) and (%f,%f)", v1.x, v1.y, v2.x, v2.y);
      game->dbg_addCellOverlay(CellOverlay(x1, y1, ORANGE));
      if (x1 != x2 && y1 != y2) {
        const EdgeInfo e(x1,y1,x2,y2);
        LOGE("edge going from (%i,%i) to (%i,%i) => ymin=%i,m=%f,b=%f", x1, y1, x2, y2, e.ymin, e.m, e.b);
        edgeTable[e.ymin].append(e);
        game->dbg_addCellOverlay(CellOverlay(0, e.b, WHITE));
        numEdges++;
        minY = MIN(minY, e.ymin);
        maxY = MAX(maxY, e.ymax);
      } else {
        if (x1 == x2 && y1 != y2) {
          //FIXME: This is a vertical edge, just fill it right now
        } else if (y1 == y2 && x1 != x2) {
          //FIXME: horizontal edge, fill right now
        } else {
          //empty edge, probably got clipped because it was lying completely outside
        }
      }
    }
  }

  //Active edges table (contains the edges that will intersect our scanline (because our current y is in their [ymin,ymax])
  list<EdgeInfo> activeEdges;

  //Start with the scanline
  for (unsigned y=minY; y<=maxY; y++) {
    //move from edge tables the edges with y = ymin (entering in active edges)
    LIST_FOREACH(EdgeInfo, edgeTable[y], i) {
      activeEdges.append(*i);
    }

    //calculate intersections
    binaryheap<int, IntCompare> intersections(10);
    for (list<EdgeInfo>::iterator i = activeEdges.begin(); i.hasNext();) {
      const EdgeInfo& e = *i;
      const int x = (int)((y-e.b)/e.m);
      LOGE("intersection with scanline y=%i and edge with m=%f, b=%f \t=> x=%i", y, e.m, e.b, x);
      if (x > 0 && x < gridW) {
        intersections.insert(x);
        //game->dbg_addCellOverlay(CellOverlay(x, y, GREEN));
      }

      //remove from active edges the edges with y = ymax
      if ((*i).ymax == y)
        i = activeEdges.remove(i);
      else
        i++;
    }

    while (intersections.size() > 0) {
      const int x1 = intersections.removeRoot();
      if (intersections.size() == 0) {
        for (int x=x1; x<gridW; x++) {
          cells[x][y]->data.visibility = HIDDEN;
        }
      } else {
        const int x2 = intersections.removeRoot();
        for (int x=x1; x<=x2; x++) {
          cells[x][y]->data.visibility = HIDDEN;
        }
      }
    }
  }


  /*const vector<ShadowPolygon*>& shadows = game->getPlayerShadows();

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
  }*/
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

Path* VisibilityGrid::pathToClosest (bool vis, int& outX, int& outY) const {
  //TODO
/*  int coords[2] = {-1,-1};
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

  outX = coords[0];
  outY = coords[1];
  //LOGE("start[%i,%i], closest[%i,%i]", djikstraStart->x, djikstraStart->y, coords[0], coords[1]);
  return pathTo(cells[coords[0]][coords[1]]);*/
  return NULL;
}

bool VisibilityGrid::walkable (const Cell* c, const Entity* entity) const {
  return !grid.containsEntity(c->x, c->y, ENTITY_WALL | ENTITY_ROCKET | ENTITY_TANK, entity);
}

