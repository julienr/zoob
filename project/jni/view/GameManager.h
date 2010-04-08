/**
 * Gamemanager manage the highlevel game states, like "playing", "menu", etc...
 */

#ifndef GAMEMANAGER_H_
#define GAMEMANAGER_H_

#include "def.h"

enum eAppState {
  STATE_PLAYING=0,
  STATE_MENU
};

class GameManager;
//Type for a function that will be called when a new game has to be started
typedef void (*startGameCallback_t) (GameManager* menu);

class GameManager {
  public:
    GameManager (startGameCallback_t cb) : newGameCB(cb), state(STATE_PLAYING), currentLevel(0) {
      //Immediatly start with the first level
      newGameCB(this);
    }
    eAppState getState () { return state; }

    size_t getCurrentLevel () {
      return currentLevel;
    }

    void drawMenu ();

    inline bool inGame () { return state == STATE_PLAYING; }


  private:
    const startGameCallback_t newGameCB;
    eAppState state;
    size_t currentLevel;
};

#endif /* GAMEMANAGER_H_ */
