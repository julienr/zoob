#ifndef MAINMENU_H_
#define MAINMENU_H_

#include "Menu.h"

class MainMenu : public Menu {
  public:
    MainMenu (GameManager* gm);
    ~MainMenu ();
    void draw ();
  protected:
    void actionPerformed (short touchedItem);
  private:
    void _initItems ();
    void _actionStart ();
    void _actionNextLvl ();
    void _actionPrevLvl ();
    void _actionNextDiff ();
    void _actionPrevDiff ();
    void _actionOptions ();

    Sprite logo;
#if !FULL_VERSION
    Sprite demo;
#endif
    Sprite easy, normal, hard;
    Vector2 logoPos, logoSize;
    Vector2 numberPos, numberSize;
    Vector2 diffPos, diffSize;
};

#endif /* MAINMENU_H_ */
