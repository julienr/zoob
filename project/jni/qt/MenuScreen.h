#ifndef ZOOB_MENU_SCREEN_H
#define ZOOB_MENU_SCREEN_H

#include <QWidget>

class MainWindow;

class MenuScreen : public QWidget {
  public:
    MenuScreen (MainWindow* parent);
};

#endif
