#ifndef ZOOB_GAME_SCREEN_H
#define ZOOB_GAME_SCREEN_H

#include "def.h"

#include <QGLWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include "input/InputManager.h"
#include "input/AndroidInputManager.h"

class MainWindow;

class GameScreen : public QGLWidget {
  public:
    GameScreen (MainWindow* parent, const char* json, const char* apkPath);

    void keyPressEvent (QKeyEvent *event);
    void mouseMoveEvent (QMouseEvent *event);
    void mousePressEvent (QMouseEvent *event);
    void mouseReleaseEvent (QMouseEvent *event);

    InputManager* createInputManager (int gamepad, int trackball) { 
      inputManager = new AndroidInputManager(gamepad, trackball);
      return inputManager;
    }

    void startGame (int level);
    void startMultiplayerGame (int level, bool server);
  protected:
    void initializeGL();
    void resizeGL (int width, int height);
    void paintGL ();
  private:
    MainWindow* mainWindow;
    AndroidInputManager* inputManager;
    
};

#endif
