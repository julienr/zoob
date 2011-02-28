#ifndef ZOOB_GAME_SCREEN_H
#define ZOOB_GAME_SCREEN_H

#include "zoobdef.h"

#include <QGLWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include "input/InputManager.h"
#include "input/AndroidInputManager.h"

class MainWindow;

class GameScreen : public QGLWidget {
  public:
    GameScreen (MainWindow* parent);

    void keyPressEvent (QKeyEvent *event);
    void mouseMoveEvent (QMouseEvent *event);
    void mousePressEvent (QMouseEvent *event);
    void mouseReleaseEvent (QMouseEvent *event);

    InputManager* createInputManager (int gamepad, int trackball) { 
      inputManager = new AndroidInputManager(gamepad, trackball);
      return inputManager;
    }

    void startGame (int level);
    void startServer (int level);
    void startClient (const char* serverAddr);
  protected:
    void initializeGL();
    void resizeGL (int width, int height);
    void paintGL ();
  private:
    MainWindow* mainWindow;
    AndroidInputManager* inputManager;
    
};

#endif
