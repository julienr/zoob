#ifndef PROGRESSIONMANAGER_H_
#define PROGRESSIONMANAGER_H_

#include "Tank.h"
#include "containers/vector.h"

enum ePlayerForm {
  FORM_SIMPLE=TANK_SIMPLE,
  FORM_BOUNCE=TANK_BOUNCE,
  FORM_BURST=TANK_BURST
};

enum eReward {
    REWARD_NONE,
    REWARD_BOMB,
    REWARD_SHIELD,
    REWARD_BOUNCE,
    REWARD_FIRING
};

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

    //Return the reward awarded by the last completed level,
    //REWARD_NONE if nothing
    eReward getLastReward () const;

  private:
    size_t _level() const;
    vector<ePlayerForm> availablePlayerForms;
};

#endif /* PROGRESSIONMANAGER_H_ */
