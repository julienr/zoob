#ifndef GAMEVIEW_H_
#define GAMEVIEW_H_

#include "view/TankView.h"
#include "view/PlayerTankView.h"
#include "view/CursorView.h"
#include "view/LevelView.h"
#include "logic/Game.h"
#include "containers/vector.h"
#include "Explosion.h"

class GameView {
  public:
    GameView (const Game& g);
    ~GameView ();

    const PlayerTankView& getTankView () const {
      return playerTankView;
    }

    //Only draw the hearts representing the life
    void drawHearts ();

    //Draw main game area
    void draw();

    void debugDraw();
  private:
    const Game& game;
    PlayerTankView playerTankView;
    //CursorView cursorView;
    LevelView levelView;
    Sprite arrowEnd;
    Sprite rocket;
    Sprite mine;
    Sprite hearthEmpty;
    Sprite hearthFull;
    vector<TankView*> enemiesView;
    list<Explosion*> explosions;
    Sprite* numbers[6];
};

#endif /* GAMEVIEW_H_ */
