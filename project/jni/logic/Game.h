#ifndef GAME_H_
#define GAME_H_

#include "def.h"
#include "logic/Tank.h"
#include "logic/Rocket.h"
#include "logic/Cursor.h"
#include "logic/Level.h"
#include "levels/LevelsData.h"
#include "logic/physics/CollisionManager.h"
#include "lib/Utils.h"
#include "containers/vector.h"

#define TANK_MOVE_SPEED 1.5f
#define ROCKET_MOVE_SPEED 2.0f

enum eMoveState {
  MOVING_NONE=0,
  MOVING_TANK,
  MOVING_CURSOR,
  MOVING_TANK_PAD, //moving tank using the gamepad
};

typedef void (*game_over_callback_t) ();

class Game {
  public:
    Game (game_over_callback_t goCallback, Level* level); //gamePadPos is in game space
    ~Game ();

    void start () {
      lastTime = Utils::getCurrentTimeMillis();
    }

    void unpause () {
      lastTime = Utils::getCurrentTimeMillis() + 100;
    }

    const Tank& getPlayerTank () const {
      return tank;
    }

    const list<Tank*>* getEnemies () const {
      return &enemies;
    }

    const Cursor& getCursor () const {
      return cursor;
    }

    void setCursorPosition (const Vector2& pos) {
      cursor.setPosition(pos);
    }

    const Level* getLevel () const {
      return level;
    }

    const CollisionManager& getColManager () const {
      return colManager;
    }

    //When moving the tank, use this to set the position of where the user touched to move the tank
    //pos is SUPPOSED to be in Game coords
    void setMoveTouchPoint (const Vector2& pos);

    const Vector2& getTankMoveTouchPoint () const {
      ASSERT(isMovingTank());
      return tankMoveEnd;
    }

    Vector2 getTankMoveDir () const {
      ASSERT(isMovingTank());
      if (movingState == MOVING_TANK)
        return tankMoveEnd - tank.getPosition();
      else if (movingState == MOVING_TANK_PAD)
        return tankMoveEnd - gamePadPos;
      else {
        ASSERT(false);
        return Vector2(0,0);
      }
    }

    void startMoving (eMoveState what, const Vector2& touchPosition);

    void setGamePadPos (const Vector2& v) { gamePadPos = v; }

    void stopMoving();

    bool isMovingTank () const {
      return movingState == MOVING_TANK || movingState == MOVING_TANK_PAD;
    }

    bool isMovingCursor () const {
      return movingState == MOVING_CURSOR;
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
    Tank tank;
    list<Tank*> enemies;
    list<Rocket*> rockets;
    list<Vector2> explosions;
    Cursor cursor;
    Level* level;

    //elapsed time for last calculated frame
    double elapsedS;

    eMoveState movingState;
    //If the move started on the virtual gamePad, we don't want to use tank.getPosition to calculate
    //the direction
    Vector2 gamePadPos;
    Vector2 tankMoveEnd;

    uint64_t lastTime;
    game_over_callback_t gameOverCallback;
};

#endif /* GAME_H_ */
