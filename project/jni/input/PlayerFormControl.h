#ifndef PLAYERFORMCONTROL_H_
#define PLAYERFORMCONTROL_H_

#include "view/menu/Menu.h"
#include "logic/PlayerTank.h"

class PlayerFormControl : public Menu {
  public:
    PlayerFormControl ();

    void draw ();
  protected:
    void actionPerformed (short touchedItem);
  private:
    void _actionNext();
    void _actionPrev();
    Sprite tankSprite;
    Sprite shieldTankSprite;
    size_t currentFormIdx;
};

#endif /* PLAYERFORMCONTROL_H_ */
