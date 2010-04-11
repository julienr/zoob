/**
 * Gamemanager manage the highlevel game states, like "playing", "menu", etc...
 */

#ifndef GAMEMANAGER_H_
#define GAMEMANAGER_H_

#include "def.h"
#include "containers/vector.h"
#include "menu/Menu.h"
#include "menu/MainMenu.h"
#include "levels/LevelsData.h"

enum eAppState {
  STATE_PLAYING=0,
  STATE_MAINMENU
};

class GameManager;
//Type for a function that will be called when a new game has to be started
typedef void (*startGameCallback_t) (GameManager* menu);

class GameManager {
  public:
    GameManager (startGameCallback_t cb)
      : newGameCB(cb),
        state(STATE_MAINMENU),
        currentLevel(0),
        mainMenu(this) {
    }

    ~GameManager () {
    }

    eAppState getState () { return state; }

    void setState (eAppState s) { state = s; }

    size_t getCurrentLevel () {
      return currentLevel;
    }

    void nextLevel () {
      if (isAtLastLevel())
        return;
      currentLevel++;
    }

    void prevLevel () {
      if (isAtFirstLevel())
        return;
      currentLevel--;
    }

    void newGame () {
      state = STATE_PLAYING;
      newGameCB(this);
    }

    inline
    bool isAtLastLevel () {
      return currentLevel == numLevels-1;
    }

    inline
    bool isAtFirstLevel () {
      return currentLevel == 0;
    }

    void drawMenu ();

    void handleTouchDown (const Vector2& p);
    void handleTouchUp (const Vector2& p);

    void resize (float sW, float sH);

    inline bool inGame () { return state == STATE_PLAYING; }


  private:
    const startGameCallback_t newGameCB;
    eAppState state;
    size_t currentLevel;
    float screenWidth, screenHeight;

    MainMenu mainMenu;
};

#endif /* GAMEMANAGER_H_ */
