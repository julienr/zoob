/**
 * Gamemanager manage the highlevel game states, like "playing", "menu", etc...
 */

#ifndef GAMEMANAGER_H_
#define GAMEMANAGER_H_

#include "zoobdef.h"
#include "containers/vector.h"
#include "menu/Menu.h"
#include "menu/PausedMenu.h"
#include "levels/LevelManager.h"
#include "logic/Game.h"

enum eAppState {
  STATE_NONE=0,
  STATE_PLAYING,
  STATE_LOST,
  STATE_WON,
  STATE_END,
  STATE_PAUSED,
  STATE_ERROR,
  MAX_STATE
};

//callback for state switch
typedef void (*callback_t) ();

class GameManager {
  public:
    static void create (callback_t gameCb, //called when new game is started
                        callback_t continueCb,
                        const LevelManager* lvlManager) { //called when unpausing game
      instance = new GameManager(gameCb, continueCb, lvlManager);
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

    GameManager (callback_t gameCb, 
                 callback_t continueCb,
                 const LevelManager* lvlManager);

    ~GameManager () {
      for (int i=0; i<MAX_STATE; i++)
        if (menus[i])
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
      initialDelay = transitionDelay;
    }

    //MUST be called by the RENDERING thread. Apply the state transition if any
    void applyTransition ();

    bool inTransition () {
      return stateTransition != -1;
    }

    //result is undefined if !isInTransition()
    float getTransitionDelay () {
      return transitionDelay/1000.0f;
    }

    float getInitialDelay () {
      return initialDelay/1000.0f;
    }

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

    void newGame () {
      newGameCB();
    }

    void nextGame () {
      ASSERT(hasMoreLevels());
      currentLevel++;
      newGameCB();
    }

    void unpause () {
      continueCB();
    }

    void setCurrentLevel (size_t level) {
      currentLevel = level;
    }

    inline
    bool isAtLevelLimit () {
      return currentLevel >= lvlManager->getNumLevels();
    }

    inline
    bool hasMoreLevels () {
      return currentLevel < LevelManager::getInstance()->getNumLevels()-1;
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

    const callback_t newGameCB;
    const callback_t continueCB;
    eAppState state;
    size_t currentLevel;

    //This is to avoid that a touchdown event that started before a state change is propagated in
    //another state
    eAppState stateAtTouchDown;

    Menu* menus[MAX_STATE];
    const LevelManager* lvlManager;

    //When input is threaded (as it is on Android), it shouldn't try to change states directly, because
    //they might involve openGL related calls (loading/unloading texture groups).
    int stateTransition;
    //by setting stateTransition and transitionDelay to something non-null, one can put a small delay on the state transition
    //There is no guarantee that the state transition won't be overrided during this delay though
    int transitionDelay;
    int initialDelay;

    //Callbacks called when entering a new state. Nothing called if NULL
    callback_t stateCallbacks[MAX_STATE];
};

#endif /* GAMEMANAGER_H_ */
