#ifndef PROGRESSIONMANAGER_H_
#define PROGRESSIONMANAGER_H_

#include "Tank.h"
#include "containers/vector.h"
#include "Level.h"

enum ePlayerForm {
  FORM_SIMPLE=TANK_SIMPLE,
  FORM_BOUNCE=TANK_BOUNCE,
  FORM_BURST=TANK_BURST
};

class PlayerTank;

/**
 * This class handle all the informations relative to the player progress through the game
 * (num levels completed, available forms)
 */
class ProgressionManager {
  /* singleton */
  public:
    static ProgressionManager* getInstance () {
      if (!instance)
        instance = new ProgressionManager();
      return instance;
    }

    static void destroy () {
      delete instance;
    }
  private:
    ProgressionManager ();
    static ProgressionManager* instance;

  /* instance */
  public:
    const vector<ePlayerForm>& getAvailablePlayerForms () {
      return availablePlayerForms;
    }

    bool hasForms () const {
      return availablePlayerForms.length() > 1;
    }

    //called when the level has changed
    void changedLevel ();

    bool hasBombs () const;

    bool hasShield () const;

    //Level is explicitely passed here because this is usually called in Game::Game() and
    //Game::getInstance() would return null (in _level()) at this point
    void setPlayerForm (Level* level, PlayerTank* player) const;

    uint64_t getPlayerFireInterval () const;
    float getPlayerRocketsSpeed () const;

    //Return the reward awarded by the last completed level,
    //REWARD_NONE if nothing
    eReward getLastReward () const;

  private:
    const Level* _level() const;
    vector<ePlayerForm> availablePlayerForms;

    //cache of available items
    bool items[4];
};

#endif /* PROGRESSIONMANAGER_H_ */
