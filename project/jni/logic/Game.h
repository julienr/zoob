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

#define TANK_MOVE_SPEED 1.5f
#define ROCKET_MOVE_SPEED 2.0f

enum eGameState {
  GAME_RUNNING,
  GAME_PAUSED
};

typedef void (*game_callback_t) ();

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

    const Tank& getPlayerTank () const {
      return tank;
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

    //Returns an iterator over the explosions location that happened
    //since last frame
    const list<Vector2>::const_iterator getExplosions () const {
      return explosions.begin();
    }

    double getLastFrameElapsed () const {
      return elapsedS;
    }

    void touch (Entity* e1, Entity* e2, const Vector2& colPoint);

    int getPlayerNumLives () const { return playerNumLives; }
    int getPlayerMaxLives () const { return playerMaxLives; }

    void update();

    void playerFire (const Vector2& cursorPosition);

    //Set tank movement direction. Set to (0,0) for now move
    void setTankMoveDir (const Vector2& dir) {
      tankMoveDir.set(dir.x,dir.y);
    }

    void toggleGodMode () {
      godMode = !godMode;
    }

  private:
    //Move and rotate the tank according to dir and calls slideMove
    void doTankMove (Tank* t, Vector2 dir, double elapsedS);

    //translate the given entity of <move>, sliding against collider
    void slideMove (Entity* e, Vector2 move);
    //translate the given rocket, bouncing against wall (and registering the bounce in the rocket)
    void bounceMove (Rocket* r, Vector2 move);

    int playerMaxLives;
    int playerNumLives;

    CollisionManager colManager;
    PlayerTank tank;
    list<EnemyTank*> enemies;
    list<Rocket*> rockets;
    list<Vector2> explosions;
    Level* level;

    //elapsed time for last calculated frame
    double elapsedS;

    Vector2 tankMoveDir;

    uint64_t lastTime;
    game_callback_t gameOverCallback;
    game_callback_t gameWonCallback;

    eGameState gameState;

    bool godMode;
};

#endif /* GAME_H_ */
