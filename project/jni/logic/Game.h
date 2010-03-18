#ifndef GAME_H_
#define GAME_H_

#include "def.h"
#include "logic/Tank.h"
#include "logic/Cursor.h"
#include "logic/Level.h"
#include "levels/LevelsData.h"
#include "logic/CollisionManager.h"
#include "lib/Utils.h"

#define TANK_MOVE_SPEED 1.0f

class Game {
  public:
    Game (Level* level) :
      level(level),movingTank(false) {
      level->addToColManager(colManager);
      colManager.addEntity(&tank);
    }

    void start () {
      lastTime = Utils::getCurrentTimeMillis();
    }

    void unpause () {
      lastTime = Utils::getCurrentTimeMillis() + 100;
    }

    const Tank& getPlayerTank () const {
      return tank;
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
    void setTankMoveTouchPoint (const Vector2& pos) {
      tankMoveEnd.set(pos);
    }

    const Vector2& getTankMoveTouchPoint () const {
      if (!movingTank)
        LOGE("getTankMovePosition called when not moving tank");
      return tankMoveEnd;
    }

    /** Switch tank moving state on/off */
    void startMovingTank (const Vector2& touchPosition) {
      movingTank = true;
      tankMoveEnd.set(touchPosition);
    }

    void stopMovingTank () {
      movingTank = false;
    }

    bool isMovingTank () const {
      return movingTank;
    }

    void update();
  private:
    CollisionManager colManager;
    Tank tank;
    Cursor cursor;
    Level* level;

    bool movingTank;
    Vector2 tankMoveEnd;

    long lastTime;
};

#endif /* GAME_H_ */
