#include "VisibilityGrid.h"
#include "containers/binaryheap.h"
#include "logic/ShadowPolygon.h"
#include "logic/Game.h"

VisibilityGrid::VisibilityGrid(const Grid& grid)
  : AbstractGrid<VisCell>(grid) {
}

struct EdgeInfo {
  int ymin,ymax;
  const bool vertical;
  //y=m*x+b edge equation
  float m;
  float b;

  int x;

  EdgeInfo(const int x1, const int y1, const int x2, const int y2)
    : vertical(false) {
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

  //special constructor for vertical edges
  EdgeInfo (const int x, const int y1, const int y2)
    : vertical(true){
    this->x = x;
    if (y2<y1) {
      ymin = y2;
      ymax = y1;
    } else {
      ymin = y1;
      ymax = y2;
    }
  }
};

struct IntersectionInfo {
    int x;
    int ymin; //ymin of the edge that caused this intersection. This is used to handle vertices corretly (count once or twice)

    IntersectionInfo (int x, int ymin)
      : x(x), ymin(ymin) {}
};

struct IntCompare {
  bool operator() (const IntersectionInfo& i1, const IntersectionInfo& i2) {
    return i1.x < i2.x;
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

  //FIXME: this could be made static
  Line levelClip[4];
  const unsigned levelW = game->getLevel()->getWidth();
  const unsigned levelH = game->getLevel()->getHeight();
  const Vector2& origin = grid.getOrigin();
  //top
  levelClip[0] = Line::fromPointAndNormal(origin, Vector2(0, -1));
  //bottom
  levelClip[2] = Line::fromPointAndNormal(origin+Vector2(0, levelH), Vector2(0, 1));
  //right
  levelClip[1] = Line::fromPointAndNormal(origin+Vector2(levelW, 0), Vector2(1, 0));
  //left
  levelClip[3] = Line::fromPointAndNormal(origin, Vector2(-1, 0));

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
      //use the bounded version because the clipping happens just on the side of the grid, so we might fall slightly out due to rounding errors
      const int x1 = grid.getCellXBounded(v1);
      const int y1 = grid.getCellYBounded(v1);
      const int x2 = grid.getCellXBounded(v2);
      const int y2 = grid.getCellYBounded(v2);
      //LOGE("verts (%f,%f) and (%f,%f)", v1.x, v1.y, v2.x, v2.y);
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
          //vertical edge => use dedicated EdgeInfo constructor
          const EdgeInfo e(x1, y1, y2);
          LOGE("vertical edge going from (%i,%i) to (%i,%i) => ymin=%i,x=%i", x1, y1, x2, y2, e.ymin, e.x);
          edgeTable[e.ymin].append(e);
          minY = MIN(minY, e.ymin);
          maxY = MAX(maxY, e.ymax);
        } else if (y1 == y2 && x1 != x2) {
          LOGE("horizontal edge going from (%i,%i) to (%i,%i) => filling immediatly", x1, y1, x2, y2);
          //horizontal edge => fill right now
          for (int x=x1; x<=x2;x++)
            cells[x][y1]->data.visibility = HIDDEN;
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
    binaryheap<IntersectionInfo, IntCompare> intersections(10);
    for (list<EdgeInfo>::iterator i = activeEdges.begin(); i.hasNext();) {
      const EdgeInfo& e = *i;
      int x;
      if (e.vertical) {
        x = e.x;
        LOGE("intersection with scanline y=%i and vertical edge with x=%i", y, e.x);
      } else {
        //FIXME: should round to ceil if on the right and to floor if on the left of the edge
        x = (int)Math::round((y-e.b)/e.m);
        LOGE("intersection with scanline y=%i and edge with m=%f, b=%f \t=> x=%i", y, e.m, e.b, x);
      }
      if (x >= 0 && x < gridW) {
        intersections.insert(IntersectionInfo(x, e.ymin));
        //game->dbg_addCellOverlay(CellOverlay(x, y, GREEN));
      }

      //remove from active edges the edges with y = ymax
      if ((*i).ymax == y)
        i = activeEdges.remove(i);
      else
        i++;
    }

    if (intersections.size() == 0) {
      LOGE("no intersections with scanline y=%i", y);
      break;
    }

    bool draw = true;
    IntersectionInfo prev = intersections.removeRoot();
    while (intersections.size() > 0) {
      IntersectionInfo cur = intersections.pickRoot();
      if (cur.x == prev.x) {
        //if the two edges that have caused the intersection are on different sides of the scanline,
        //we should count the intersection once (ie: keep it)
        //if the two edges are on the same side, just discard this intersection and proceed

        if (Math::signum(cur.ymin - (int) y) == Math::signum(prev.ymin - (int) y)) { //same side => discard
          intersections.removeRoot();
        } //else, different sides => keep one
      } else {
        if (draw) {
          for (int x = prev.x; x <= cur.x; x++) {
            cells[x][y]->data.visibility = HIDDEN;
          }
        }
        intersections.removeRoot();
      }
      draw = !draw;
      prev = cur;
    }
    if (draw) {
      for (int x=prev.x; x<gridW; x++) {
        cells[x][y]->data.visibility = HIDDEN;
      }
    }
#if 0
    //clean up vertices that created two intersections
    binaryheap<IntersectionInfo, IntCompare> cleanedIntersections(10);
    while (intersections.size() > 0) {
      const IntersectionInfo x1 = intersections.removeRoot();
      if (intersections.size() > 0) {
        const IntersectionInfo& x2 = intersections.removeRoot();
        //x1 and x2 might be two intersections caused by the same vertice
        if (x1.x == x2.x) {
          //if the two edges that have caused the intersection are on different sides of the scanline,
          //we should count the intersection once (ie: keep it)
          //if the two edges are on the same side, just discard this intersection and proceed
          if (Math::signum(x1.ymin-(int)y) != Math::signum(x2.ymin-(int)y)) { //different sides => keep one
            cleanedIntersections.insert(x1);
            LOGE("keeping vertice intersection at x=%i", x1.x);
          } else {
            LOGE("droping vertice intersection at x=%i", x1.x);
          }
        } else {
          cleanedIntersections.insert(x1);
          cleanedIntersections.insert(x2);
        }
      } else {
        cleanedIntersections.insert(x1);
      }
    }

    LOGE("cleanedIntersection : %i", cleanedIntersections.size());

    if (cleanedIntersections.size() == 0)
      continue;

    bool draw = true;
    IntersectionInfo prev = cleanedIntersections.removeRoot();
    while (cleanedIntersections.size() > 0) {
      IntersectionInfo cur = cleanedIntersections.removeRoot();
      LOGE("prev (x=%i), cur (x=%i)", prev.x, cur.x);
      if (draw) {
        for (int x = prev.x; x < cur.x; x++) {
          cells[x][y]->data.visibility = HIDDEN;
        }
      }
      draw = !draw;
      prev = cur;
    }
    if (draw) {
      LOGE("at last intersection");
      for (int x = prev.x; x < gridW; x++) {
        cells[x][y]->data.visibility = HIDDEN;
      }
    }
#endif
    /*while (cleanedIntersections.size() > 0) {
      const IntersectionInfo x1 = cleanedIntersections.removeRoot();

      if (cleanedIntersections.size() == 0) {
        LOGE("at last intersection");
        for (int x = x1.x; x < gridW; x++) {
          cells[x][y]->data.visibility = HIDDEN;
        }
      } else {
        const IntersectionInfo& x2 = cleanedIntersections.removeRoot();
        ASSERT(x1.x != x2.x); //should have been handled previously
        for (int x = x1.x; x <= x2.x; x++) {
          cells[x][y]->data.visibility = HIDDEN;
        }
      }
    }*/

    /*while (intersections.size() > 0) {
      const IntersectionInfo x1 = intersections.removeRoot();

      if (intersections.size() == 0) {
        LOGE("at last intersection");
        for (int x=x1.x; x<gridW; x++) {
          cells[x][y]->data.visibility = HIDDEN;
        }
      } else {
        const IntersectionInfo& x2 = intersections.pickRoot();
        //x1 and x2 might be two intersections caused by the same vertice
        if (x1.x == x2.x) {
          //if the two edges that have caused the intersection are on different sides of the scanline,
          //we should count the intersection once (ie: keep it)
          //if the two edges are on the same side, just discard this intersection and proceed

          if (Math::signum(x1.ymin-(int)y) == Math::signum(x2.ymin-(int)y)) { //same side => discard
            intersections.removeRoot();
          } //else, different sides => keep one
        } else {
          for (int x=x1.x; x<=x2.x; x++) {
            cells[x][y]->data.visibility = HIDDEN;
          }
        }
      }
    }*/
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

bool VisibilityGrid::walkable (const Cell* c, const Entity* entity) const {
  return !grid.containsEntity(c->x, c->y, ENTITY_WALL | ENTITY_ROCKET | ENTITY_TANK, entity);
}

