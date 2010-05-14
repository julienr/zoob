#ifndef VISIBILITYGRID_H_
#define VISIBILITYGRID_H_

#include "logic/physics/Grid.h"
#include "logic/Path.h"
#include "containers/binaryheap.h"
#include "logic/AbstractGrid.h"
class Game;

struct VisCell {
  VisCell () : visible(true) {}
  bool visible;

  AbstractGrid<VisCell>::Cell* parent;
  bool closed;
  int dist;

  void reset () {
    parent = NULL;
    closed = false;
    dist = INT_INF;
  }
};

class VisibilityGrid : public AbstractGrid<VisCell> {
  friend class Game;
  public:
    VisibilityGrid(const Grid& grid);

    void djikstra (const Vector2& startPos, const Entity* source);

    //Returns the shortest path to the closest hidden cell
    //ASSUME djikstra has been called first (startPos is the one given to djikstra)
    Path* pathToClosestHidden () const;

    void print () const;

    bool isVisible (int x, int y) const {
      assert(inside(x,y));
      return cells[x][y]->data.visible;
    }

    bool isWalkable (int x, int y) const {
      return walkable(cells[x][y]);
    }

  protected:
    void calculateVisibility (const Game* game);

    bool walkable (const Cell* c) const;
    void _resetCells ();
    static int neighDist (const Cell* c1, const Cell* c2);

    Cell* djikstraStart; //contains the last cell used to start a djikstra
    const Entity* djikstraSource; //source entity for djikstra
};

#endif
