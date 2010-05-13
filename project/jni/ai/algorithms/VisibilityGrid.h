#ifndef VISIBILITYGRID_H_
#define VISIBILITYGRID_H_

#include "logic/physics/Grid.h"
#include "logic/Path.h"
#include "containers/binaryheap.h"
#include "ai/algorithms/AbstractGrid.h"
class Game;

struct CellData {
  CellData () : visible(true) {}
  bool visible;

  AbstractGrid<CellData>::Cell* parent;
  bool closed;
  int dist;

  void reset () {
    parent = NULL;
    closed = false;
    dist = INT_INF;
  }
};

class VisibilityGrid : public AbstractGrid<CellData> {
  public:
    VisibilityGrid(const Grid& grid);

    void djikstra (const Vector2& startPos, const Entity* source);

    void calculateVisibility (const Game* game);

    //Returns the shortest path to the closest hidden cell
    //ASSUME djikstra has been called first (startPos is the one given to djikstra)
    Path* pathToClosestHidden ();

    void print ();

  protected:
    bool walkable (const Cell* c);
    void _resetCells ();
    static int neighDist (const Cell* c1, const Cell* c2);

    Cell* djikstraStart; //contains the last cell used to start a djikstra
    const Entity* djikstraSource; //source entity for djikstra
};

#endif
