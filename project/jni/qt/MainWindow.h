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
    MainWindow (const char* serieJSON, const char* apkPath);
    virtual ~MainWindow ();

    InputManager* createInputManager (int gamepad, int trackball) { return gameScreen->createInputManager(gamepad, trackball); }

    void showMenu (eMenu menu, int currentLevel=-1);

  public slots:
    void startGame (int level=-1);
    void mainLoop ();

    void quit ();

    void debugChanged (eDebug what, bool enabled);

  private:
    void createActions ();
    void createMenus ();
    void restoreSettings ();

    MenuScreen* menuScreen;
    GameScreen* gameScreen;
    InterMenu* menuInter;
    QStackedLayout* layout;

    QTime timer;

    bool running;

    QSettings settings;

    //Maps a debug option settings name to the debug flag
    //Used for settings
    QHash<QString, eDebug> debugOptions;

    //menu actions
    QMap<QString, DebugAction*> debugActions;
};

#endif
