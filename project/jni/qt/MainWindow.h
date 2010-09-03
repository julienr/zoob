#ifndef ZOOB_MAIN_WINDOW_H
#define ZOOB_MAIN_WINDOW_H

#include "MenuScreen.h"
#include "GameScreen.h"

#include <QWidget>
#include <QStackedLayout>
#include <QTime>
#include "input/InputManager.h"


class MainWindow : public QWidget {
  Q_OBJECT
  public:
    MainWindow (const char* serieJSON, const char* apkPath);
    virtual ~MainWindow ();

    InputManager* createInputManager (int gamepad, int trackball) { return gameScreen->createInputManager(gamepad, trackball); }

  public slots:
    void startGame ();
    void showMenu ();

    void mainLoop ();

    void quit ();

  private:
    MenuScreen* menuScreen;
    GameScreen* gameScreen;
    QStackedLayout* layout;

    QTime timer;

    bool running;
};

#endif
