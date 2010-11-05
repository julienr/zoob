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
    static void drawAABBox (const AABBox* box, const Vector2& position);
    static void drawBCircle (const BCircle* circle, const Vector2& position);
  public:
    GameView ();
    ~GameView ();

    //Only draw the hearts representing the life
    void drawHearts () const;

    void drawLevelIndicator () const;

    //Draw main game area
    void draw(double elapsedS);

    //Debug draw
    void debugShadows();
    void debugAI ();
    void debugCollisions();
    void debugVisibility();
    void debugOverlays ();
  private:
    void _drawGame (double elapsedS);
    void _drawBossIntro(double elapsedS);
    //Draw a lighting gradient around the player tank
    void _drawLighting() const;
    //Draw the shadows projected by the walls
    void _drawShadows() const;

    //CursorView cursorView;
    LevelView levelView;
    Sprite rocket;
    Sprite bomb;
    Sprite hearthEmpty;
    Sprite hearthFull;
    Sprite circle;
    vector<TankView*> tankViews;
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
