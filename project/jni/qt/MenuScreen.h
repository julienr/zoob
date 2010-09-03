#ifndef ZOOB_MENU_SCREEN_H
#define ZOOB_MENU_SCREEN_H

#include <QWidget>
#include <QSpinBox>

class MainWindow;

class MenuScreen : public QWidget {
  public:
    MenuScreen (MainWindow* parent);

    void setLevelsRange (int min, int max);

    int getSelectedLevel ();

    void selectLevel (int level);
    void nextLevel ();
  private:
    QSpinBox* levelsSpin;
};

#endif
