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
    GameView (Game& g);
    ~GameView ();

    const PlayerTankView& getTankView () const {
      return playerTankView;
    }

    //Only draw the hearts representing the life
    void drawHearts () const;

    void drawLevelIndicator () const;

    //Draw main game area
    void draw() {
      if (game.inIntro())
        _drawBossIntro();
      else
        _drawGame();
    }

    void debugDraw();
    void debugDrawAI();
  private:
    void _drawGame ();
    void _drawBossIntro();
    //Draw a lighting gradient around the player tank
    void _drawLighting() const;
    //Draw the shadows projected by the walls
    void _drawShadows() const;
    Game& game;
    PlayerTankView playerTankView;
    //CursorView cursorView;
    LevelView levelView;
    Sprite arrowEnd;
    Sprite rocket;
    Sprite bomb;
    Sprite hearthEmpty;
    Sprite hearthFull;
    Sprite circle;
    vector<TankView*> enemiesView;
    list<Explosion*> explosions;

    Sprite shadow;
    Sprite light;

    Sprite levelTxt;
    Sprite wtf;

    //Used for boss intro
    double lastLightToggle;
    bool lightOn;

};

#endif /* GAMEVIEW_H_ */
