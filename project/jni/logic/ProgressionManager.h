#ifndef PROGRESSIONMANAGER_H_
#define PROGRESSIONMANAGER_H_

#include "Tank.h"
#include "containers/vector.h"

enum ePlayerForm {
  FORM_SIMPLE=TANK_SIMPLE,
  FORM_BOUNCE=TANK_BOUNCE,
  FORM_BURST=TANK_BURST
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

    bool hasForms () {
      return availablePlayerForms.length() > 1;
    }

    //called when the level has changed
    void changedLevel ();

    bool hasBombs ();

    bool hasShield ();
  private:
    size_t _level();
    vector<ePlayerForm> availablePlayerForms;
};

#endif /* PROGRESSIONMANAGER_H_ */
