#ifndef CONTROLOPTIONMENU_H_
#define CONTROLOPTIONMENU_H_

#include "Menu.h"
#include "Checkbox.h"

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
    void _actionTrackballToggle ();

    Sprite inputTouch;
    Sprite inputGamepad;
    Sprite optionDesc;
    Sprite trackballDesc;
    Sprite star;

    Checkbox* trackballCheckbox;

};

#endif /* CONTROLOPTIONMENU_H_ */
