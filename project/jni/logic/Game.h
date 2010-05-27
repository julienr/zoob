#ifndef GAME_H_
#define GAME_H_

#include "def.h"
#include "logic/Tank.h"
#include "logic/EnemyTank.h"
#include "logic/PlayerTank.h"
#include "logic/Rocket.h"
#include "logic/Level.h"
#include "levels/LevelsData.h"
#include "logic/physics/CollisionManager.h"
#include "lib/Utils.h"
#include "containers/vector.h"
#include "logic/ShadowPolygon.h"
#include "logic/VisibilityGrid.h"

#define TANK_MOVE_SPEED 1.5f
#define ROCKET_MOVE_SPEED 2.0f

//Each boss level start with an intro. This is the duration of the intro
#define BOSS_INTRO_TIME 3

enum eGameState {
  GAME_RUNNING,
  GAME_PAUSED
};

typedef void (*game_callback_t) ();

struct ExplosionLocation {
    enum eType {
        EXPLOSION_BOOM,
        EXPLOSION_POOF
    };
    ExplosionLocation (const Vector2& p, eType t)
      : position(p), type(t) {}
    Vector2 position;
    eType type;
};

class Game {
  public:
    //overCallback : the function to call when game is over
    //wonCallback : the function to call when game is won
    Game (game_callback_t overCallback, 
          game_callback_t wonCallback, 
          Level* level); //gamePadPos is in game space
    ~Game ();

    void unpause () {
      gameState = GAME_RUNNING;
      lastTime = Utils::getCurrentTimeMillis() + 100;
    }

    void pause () {
      gameState = GAME_PAUSED;
    }

    PlayerTank* getPlayerTank () const {
      return playerTank;
    }

    const list<EnemyTank*>* getEnemies () const {
      return &enemies;
    }

    const Level* getLevel () const {
      return level;
    }

    const CollisionManager& getColManager () const {
      return colManager;
    }

    const list<Rocket*>::const_iterator getRockets () const {
      return rockets.begin();
    }
    
    const list<Bomb*>::const_iterator getMines () const {
      return bombs.begin();
    }

    const vector<ShadowPolygon*>& getPlayerShadows () const {
      return playerShadows;
    }

    //Returns the explosions that have to be displayed. The caller should delete the explosions
    //hes has handled
    list<ExplosionLocation>& getExplosions () {
      return explosions;
    }

    double getLastFrameElapsed () const {
      return elapsedS;
    }

    void touch (Entity* e1, Entity* e2, const Vector2& colPoint);

    void update();

    void playerFire (const Vector2& cursorPosition);
    void playerDropBomb ();

    //Set tank movement direction. Set to (0,0) for now move
    void setTankMoveDir (const Vector2& dir) {
      tankMoveDir.set(dir.x,dir.y);
    }

    void toggleGodMode () {
      godMode = !godMode;
    }

    bool hasShadows () const {
      return calculateShadows;
    }

    VisibilityGrid& getPlayerVisibility () {
      return playerVisibility;
    }

    const VisibilityGrid& getPlayerVisibility () const {
      return playerVisibility;
    }

    bool inIntro () const {
      return !introDone;
    }

    double getIntroTimeLeft () const {
      return introTimeLeft;
    }

  private:
    void _updateRockets (double elapsedS);
    void _updateBombs (double elapsedS);
    int _updateEnemies (double elapsedS);
    void _updatePlayer (double elapsedS);

    //Move and rotate the tank according to dir and calls slideMove
    void doTankMove (Tank* t, Vector2 dir, double elapsedS);

    void doFireRocket (Tank* t, const Vector2& dir);

    //translate the given entity of <move>, sliding against collider
    void slideMove (Entity* e, Vector2 move);
    //translate the given rocket, bouncing against wall (and registering the bounce in the rocket)
    void bounceMove (Rocket* r, Vector2 move);

    void _calculatePlayerShadows ();

    CollisionManager colManager;
    PlayerTank* playerTank;
    list<EnemyTank*> enemies;
    list<Rocket*> rockets;
    list<Bomb*> bombs;
    list<ExplosionLocation> explosions;
    Level* level;

    //elapsed time for last calculated frame
    double elapsedS;

    Vector2 tankMoveDir;

    uint64_t lastTime;
    game_callback_t gameOverCallback;
    game_callback_t gameWonCallback;

    eGameState gameState;

    bool godMode;

    //shadows calculated using the player as a light source
    vector<ShadowPolygon*> playerShadows;
    //Shadow calculations might be disabled
    const bool calculateShadows;

    VisibilityGrid playerVisibility;

    double introTimeLeft;
    bool introDone;
};

#endif /* GAME_H_ */
