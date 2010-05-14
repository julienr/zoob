#ifndef VISIBILITYGRID_H_
#define VISIBILITYGRID_H_

#include "logic/physics/Grid.h"
#include "logic/Path.h"
#include "containers/binaryheap.h"
#include "logic/AbstractGrid.h"
class Game;

enum eVisibility {
    HIDDEN=0,
    VISIBLE,
    PENUMBRA //half-hidden
};

struct VisCell {
  VisCell () : visibility(VISIBLE) {}
  eVisibility visibility;

  AbstractGrid<VisCell>::Cell* parent;
  bool closed;
  int dist;
  /** This is is the waypoint (in game coords) in this cell a unit should target
   * when passing through this cell. By default, the waypoint is the cell center, but
   * for cells close to obstacles, it might be the vertex opposing the obstacle for example
   */
  Vector2 waypoint;

  void reset (const Vector2& center) {
    parent = NULL;
    closed = false;
    dist = INT_INF;
    waypoint = center;
  }
};

//FIXME: separate djikstra and visibility grid ? (create a new Djikstra class ?)
class VisibilityGrid : public AbstractGrid<VisCell> {
  friend class Game;
  public:
    VisibilityGrid(const Grid& grid);

    void djikstra (const Vector2& startPos, const Entity* source);

    //Returns the shortest path to the closest hidden or visible cell (depending on parameter)
    //if visibility is true, return path to visible
    //ASSUME djikstra has been called first (startPos is the one given to djikstra)
    Path* pathToClosest (bool visibility) const;


    //Returns path to the center of the biggest hidden cells group
    Path* pathToCenterBiggestHidden () const;

    void print () const;

    eVisibility getVisibility (int x, int y) const {
      ASSERT(inside(x,y));
      return cells[x][y]->data.visibility;
    }

    bool isWalkable (int x, int y) const {
      return walkable(cells[x][y]);
    }

    const Vector2& getWaypoint (int x, int y) const {
      ASSERT(inside(x,y));
      return cells[x][y]->data.waypoint;
    }

  protected:
    void calculateVisibility (const Game* game);

    //source is the source given in djikstra
    Path* pathTo (const Cell* dest) const;

    bool walkable (const Cell* c) const;
    void _resetCells ();

    void _adaptWaypoints (float unitSize);
    void _adaptWaypoint (const Cell* c, float unitSize);
    static int neighDist (const Cell* c1, const Cell* c2);

    Cell* djikstraStart; //contains the last cell used to start a djikstra
    const Entity* djikstraSource; //source entity for djikstra
};

#endif
