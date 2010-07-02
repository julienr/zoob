#ifndef CONTROLOPTIONMENU_H_
#define CONTROLOPTIONMENU_H_

#include "Menu.h"

class ControlOptionMenu : public Menu {
  public:
    ControlOptionMenu (GameManager* gm);
    ~ControlOptionMenu ();
    void draw ();
  protected:
    void actionPerformed (short touchedItem);
  private:
    void _initItems ();

    void _actionBack ();
    void _actionNextCtrl ();
    void _actionPrevCtrl ();

    Sprite inputTouch;
    Sprite inputTrackball;
    Sprite inputMixed;
    Sprite optionDesc;

};

#endif /* CONTROLOPTIONMENU_H_ */
