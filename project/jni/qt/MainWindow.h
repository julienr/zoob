#ifndef ZOOB_MAIN_WINDOW_H
#define ZOOB_MAIN_WINDOW_H

#include "MenuScreen.h"
#include "GameScreen.h"
#include "InterMenu.h"

#include "app.h"

#include <QWidget>
#include <QStackedLayout>
#include <QTime>
#include <QMainWindow>
#include <QAction>
#include "DebugAction.h"
#include <QMenu>
#include <QSettings>
#include "input/InputManager.h"
#include <QHash>
#include <QMap>

class MainWindow : public QMainWindow {
  Q_OBJECT
  public:
    MainWindow ();
    virtual ~MainWindow ();

    InputManager* createInputManager (int gamepad, int trackball) { return gameScreen->createInputManager(gamepad, trackball); }

    void showMenu (AppInterface::eMenu menu, int currentLevel=-1);

    void selectLevel (int lvl) { menuScreen->selectLevel(lvl); }
    void setServer () { menuScreen->setGameType(SERVER); }
    void setClient () { menuScreen->setGameType(CLIENT); }

  public slots:
    void startGame ();
    void mainLoop ();

    void quit ();

    void debugChanged (AppInterface::eDebug what, bool enabled);

  private:
    void createActions ();
    void createMenus ();
    void restoreSettings ();

    MenuScreen* menuScreen;
    GameScreen* gameScreen;
    InterMenu* menuInter;
    QStackedLayout* layout;
    QStatusBar* statusBar;

    QTime timer;

    bool running;

    QSettings settings;

    //Maps a debug option settings name to the debug flag
    //Used for settings
    QHash<QString, AppInterface::eDebug> debugOptions;

    //menu actions
    QMap<QString, DebugAction*> debugActions;
};

#endif
