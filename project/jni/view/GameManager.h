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
#include "menu/WonMenu.h"
#include "menu/EndMenu.h"
#include "menu/PausedMenu.h"
#include "menu/BuyFullMenu.h"
#include "menu/TutorialMenu.h"
#include "levels/LevelsData.h"
#include "logic/Game.h"

enum eAppState {
  STATE_PLAYING=0,
  STATE_MAINMENU,
  STATE_LOST,
  STATE_WON,
  STATE_END,
  STATE_PAUSED,
  STATE_TUTORIAL,
  STATE_BUY_FULL,
  STATE_REWARD,
  MAX_STATE
};

class GameManager;
//Type for a function that will be called when a new game has to be started
typedef void (*startGameCallback_t) (GameManager* menu);
typedef void (*callback_t) ();

class GameManager {
  public:
    static void create (startGameCallback_t gameCb,
                          callback_t menuCb,
                          callback_t continueCb,
                          int levelLimit) {
      instance = new GameManager(gameCb, menuCb, continueCb, levelLimit);
    }

    inline
    static GameManager* getInstance() {
      ASSERT(instance);
      return instance;
    }

    static void destroy () {
      delete instance;
    }

    static bool created () {
      return instance != NULL;
    }
  private:
    static GameManager* instance;

    GameManager (startGameCallback_t gameCb, 
                 callback_t menuCb,
                 callback_t continueCb,
                 int levelLimit);

    ~GameManager () {
      for (int i=0; i<MAX_STATE; i++)
          delete menus[i];
    }
  public:
    eAppState getState () { return state; }

    void setState (eAppState s, int delay=0) {
      //If we're already transitioning to this state, discards. This is needed otherwise, when applying
      //a dely, we might run in an infinite loop if setState is called multiple time with the same state
      //and delay
      if (stateTransition == s)
        return;
      stateTransition = s;
      transitionDelay = delay;
    }

    //MUST be called by the RENDERING thread. Apply the state transition if any
    void applyTransition ();

    void setStateCallback (eAppState state, callback_t cb) {
      stateCallbacks[state] = cb;
    }

    int getCurrentTransition () const {
      return stateTransition;
    }

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

    void goToMenu () {
      menuCB();
    }

    void newGame () {
      newGameCB(this);
    }

    void nextGame () {
      ASSERT(hasMoreLevels());
      currentLevel++;
      LOGE("next Game");
      levelLimit++;
      newGameCB(this);
    }

    void unpause () {
      continueCB();
    }

    //Based on this player's progress, the biggest completed level to date
    void setLevelLimit (size_t level) {
      levelLimit = level;
    }

    inline
    bool isAtLevelLimit () {
      return currentLevel >= levelLimit;
    }

    inline
    bool hasMoreLevels () {
      return currentLevel < numLevels-1;
    }

    inline
    bool isAtLastLevel () {
      return !hasMoreLevels() || isAtLevelLimit();
      //return currentLevel == numLevels-1;
    }

    inline
    bool isAtFirstLevel () {
      return currentLevel == 0;
    }

    void drawMenu ();

    void handleTouchDown (const Vector2& p);
    void handleTouchUp (const Vector2& p);

    inline bool inGame () { return state == STATE_PLAYING; }
    inline bool paused () { return state == STATE_PAUSED; }

  private:
    //apply texture locks required by state s
    void applyLocks ();

    const startGameCallback_t newGameCB;
    const callback_t menuCB;
    const callback_t continueCB;
    eAppState state;
    size_t currentLevel;

    //This is to avoid that a touchdown event that started before a state change is propagated in
    //another state
    eAppState stateAtTouchDown;

    Menu* menus[MAX_STATE];
    size_t levelLimit;

    //When input is threaded (as it is on Android), it shouldn't try to change states directly, because
    //they might involve openGL related calls (loading/unloading texture groups).
    int stateTransition;
    //by setting stateTransition and transitionDelay to something non-null, one can put a small delay on the state transition
    //There is no guarantee that the state transition won't be overrided during this delay though
    int transitionDelay;

    //Callbacks called when entering a new state. Nothing called if NULL
    callback_t stateCallbacks[MAX_STATE];
};

#endif /* GAMEMANAGER_H_ */
