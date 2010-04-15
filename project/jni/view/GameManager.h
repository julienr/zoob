/**
 * Gamemanager manage the highlevel game states, like "playing", "menu", etc...
 */

#ifndef GAMEMANAGER_H_
#define GAMEMANAGER_H_

#include "def.h"
#include "containers/vector.h"
#include "menu/Menu.h"
#include "menu/MainMenu.h"
#include "menu/LostMenu.h"
#include "levels/LevelsData.h"

enum eAppState {
  STATE_PLAYING=0,
  STATE_MAINMENU,
  STATE_LOST,
  MAX_STATE
};

class GameManager;
//Type for a function that will be called when a new game has to be started
typedef void (*startGameCallback_t) (GameManager* menu);

class GameManager {
  public:
    GameManager (startGameCallback_t cb)
      : newGameCB(cb),
        state(STATE_MAINMENU),
        currentLevel(0) {
      menus[STATE_PLAYING] = NULL;
      menus[STATE_MAINMENU] = new MainMenu(this);
      menus[STATE_LOST] = new LostMenu(this);
    }

    ~GameManager () {
      for (int i=0; i<MAX_STATE; i++)
          delete menus[i];
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

    inline bool inGame () { return state == STATE_PLAYING; }


  private:
    const startGameCallback_t newGameCB;
    eAppState state;
    size_t currentLevel;

    //This is to avoid that a touchdown event that started before a state change is propagated in
    //another state
    eAppState stateAtTouchDown;

    Menu* menus[MAX_STATE];
};

#endif /* GAMEMANAGER_H_ */
