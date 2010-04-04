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

#define TANK_MOVE_SPEED 1.0f
#define ROCKET_MOVE_SPEED 0.5f

enum eMoveState {
  MOVING_NONE=0,
  MOVING_TANK,
  MOVING_CURSOR
};

class Game {
  public:
    Game (Level* level);
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

    const vector<Tank*>& getEnemies () const {
      return enemies;
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
      if (movingState != MOVING_TANK)
        LOGE("getTankMovePosition called when not moving tank");
      return tankMoveEnd;
    }

    void startMoving (eMoveState what, const Vector2& touchPosition);

    void stopMoving();

    bool isMovingTank () const {
      return movingState == MOVING_TANK;
    }

    const list<Rocket*>::const_iterator getRockets () const {
      return rockets.begin();
    }

    void update();
  private:
    //translate the given entity of <move>, sliding against collider
    void slideMove (Entity* e, Vector2 move);
    //translate the given rocket, bouncing against wall (and registering the bounce in the rocket)
    void bounceMove (Rocket* r, Vector2 move);


    CollisionManager colManager;
    Tank tank;
    vector<Tank*> enemies;
    list<Rocket*> rockets;
    Cursor cursor;
    Level* level;

    eMoveState movingState;
    Vector2 tankMoveEnd;

    uint64_t lastTime;
};

#endif /* GAME_H_ */
