#ifndef MAINMENU_H_
#define MAINMENU_H_

#include "Menu.h"

class MainMenu : public Menu {
  public:
    MainMenu (GameManager* gm);
    ~MainMenu ();
    void draw ();
    void resize (float sW, float sH);
  protected:
    void actionPerformed (short touchedItem);
  private:
    void _actionStart ();
    void _actionOptions ();
    void _actionNext ();
    void _actionPrev ();

    Sprite* numbers[4];
    Sprite logo;
    Vector2 logoPos, logoSize;
    Vector2 numberPos, numberSize;
};

#endif /* MAINMENU_H_ */
