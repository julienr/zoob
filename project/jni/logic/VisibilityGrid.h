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
};

//FIXME: separate djikstra and visibility grid ? (create a new Djikstra class ?)
class VisibilityGrid : public AbstractGrid<VisCell> {
  friend class Game;
  public:
    VisibilityGrid(const Grid& grid);

    //TODO: This is simply an Astar going from the tank to the player with an early stopping on the first
    //visible cell
    Path* pathToClosest (bool visibility, int& outX, int& outY) const;

    //Fill (outX, outY) with the center of the biggest area invisible to the player
    //Returns false if no bigget group is found (whole map visible)
    bool findCenterBiggestHidden (const Entity* source, int& outX, int& outY) const;

    eVisibility getVisibility (int x, int y) const {
      ASSERT(inside(x,y));
      return cells[x][y]->data.visibility;
    }

    bool isWalkable (int x, int y) const {
      return walkable(cells[x][y], NULL);
    }
  protected:
    void calculateVisibility (Game* game);

    bool walkable (const Cell* c, const Entity* e) const;
};

#endif
