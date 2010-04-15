#ifndef GAMEVIEW_H_
#define GAMEVIEW_H_

#include "view/TankView.h"
#include "view/CursorView.h"
#include "view/LevelView.h"
#include "logic/Game.h"
#include "containers/vector.h"
#include "Explosion.h"

class GameView {
  public:
    GameView (const Game& g);
    ~GameView ();

    const TankView& getTankView () const {
      return tankView;
    }

    //Only draw the hearts representing the life
    void drawHearts ();

    //Draw main game area
    void draw();

    void debugDraw();
  private:
    const Game& game;
    TankView tankView;
    CursorView cursorView;
    LevelView levelView;
    Sprite arrowEnd;
    Sprite rocket;
    Sprite hearthEmpty;
    Sprite hearthFull;
    vector<TankView*> enemiesView;
    list<Explosion*> explosions;
};

#endif /* GAMEVIEW_H_ */
