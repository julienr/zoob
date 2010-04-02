#ifndef GAMEVIEW_H_
#define GAMEVIEW_H_

#include "view/TankView.h"
#include "view/CursorView.h"
#include "view/LevelView.h"
#include "logic/Game.h"
#include "containers/vector.h"

class GameView {
  public:
    GameView (const Game& g);
    ~GameView ();

    const TankView& getTankView () const {
      return tankView;
    }

    void draw();

    void debugDraw();
  private:
    const Game& game;
    TankView tankView;
    CursorView cursorView;
    LevelView levelView;
    Sprite arrowEnd;
    Sprite rocket;
    vector<TankView*> enemiesView;
};

#endif /* GAMEVIEW_H_ */
