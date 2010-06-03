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
#include "levels/LevelsData.h"
#include "logic/Game.h"

enum eAppState {
  STATE_PLAYING=0,
  STATE_MAINMENU,
  STATE_LOST,
  STATE_WON,
  STATE_END,
  STATE_PAUSED,
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
                 int levelLimit)
      : newGameCB(gameCb),
        menuCB(menuCb),
        continueCB(continueCb),
        currentLevel(levelLimit),
        levelLimit(levelLimit) {
      menus[STATE_PLAYING] = NULL;
      menus[STATE_MAINMENU] = new MainMenu(this);
      menus[STATE_LOST] = new LostMenu(this);
      menus[STATE_WON] = new WonMenu(this);
      menus[STATE_END] = new EndMenu(this);
      menus[STATE_PAUSED] = new PausedMenu(this);
      setState(STATE_MAINMENU);
    }

    ~GameManager () {
      for (int i=0; i<MAX_STATE; i++)
          delete menus[i];
    }
  public:
    eAppState getState () { return state; }

    void setState (eAppState s);

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

    Game* getGame () {
      return currentGame;
    }

    void setGame (Game* g) {
      currentGame = g;
    }

  private:
    const startGameCallback_t newGameCB;
    const callback_t menuCB;
    const callback_t continueCB;
    eAppState state;
    size_t currentLevel;

    //This is to avoid that a touchdown event that started before a state change is propagated in
    //another state
    eAppState stateAtTouchDown;

    Menu* menus[MAX_STATE];

    //pointer to the current game, NULL if not in game
    Game* currentGame;

    size_t levelLimit;
};

#endif /* GAMEMANAGER_H_ */
