#ifndef GAMEVIEW_H_
#define GAMEVIEW_H_

#include "view/TankView.h"
#include "view/CursorView.h"
#include "view/LevelView.h"
#include "logic/Game.h"

class GameView {
  public:
    GameView (const Game& g)
      :game(g),
       tankView(g.getPlayerTank()),
       cursorView(g.getCursor()),
       levelView(g.getLevel()),
       arrowEnd("assets/sprites/arrow_end.png"){
    }

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
};

#endif /* GAMEVIEW_H_ */
