#ifndef PLAYERFORMCONTROL_H_
#define PLAYERFORMCONTROL_H_

#include "view/menu/Menu.h"
#include "logic/PlayerTank.h"

//This is the displayed control that allow the player to change the player form
class PlayerFormControl : public Menu {
  public:
    PlayerFormControl ();

    void draw ();
    void reset ();
  protected:
    void actionPerformed (short touchedItem);
  private:
    void _actionNext();
    void _actionPrev();
    Sprite tankSprite;
    Sprite bounceTankSprite;
    size_t currentFormIdx;
};

#endif /* PLAYERFORMCONTROL_H_ */
