#ifndef ZOOB_MENU_SCREEN_H
#define ZOOB_MENU_SCREEN_H

#include <QWidget>
#include <QSpinBox>
#include <QComboBox>

class MainWindow;

enum eGameType {
  LOCAL=0,
  SERVER=1,
  CLIENT=2
};

class MenuScreen : public QWidget {
  public:
    MenuScreen (MainWindow* parent);

    void setLevelsRange (int min, int max);

    int getSelectedLevel ();
    eGameType getGameType();

    void selectLevel (int level);
    void nextLevel ();
  private:
    QSpinBox* levelsSpin;
    QComboBox* gameType;
};

#endif
