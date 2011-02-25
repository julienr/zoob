#include "GameScreen.h"
#include "MainWindow.h"
#include "app.h"

GameScreen::GameScreen (MainWindow* parent, const char* json) :
  QGLWidget(QGLFormat(QGL::DoubleBuffer |
                      QGL::DepthBuffer |
                      QGL::Rgba |
                      QGL::AlphaChannel |
                      QGL::AccumBuffer |
                      QGL::StencilBuffer |
                      QGL::DirectRendering |
                      QGL::NoSampleBuffers)),
  mainWindow(parent) {
  getApp()->init(json);
}

void GameScreen::keyPressEvent (QKeyEvent* event) {
  LOGE("keyPress");
  switch (event->key()) {
    case Qt::Key_Escape:
      mainWindow->showMenu(AppInterface::MENU_MAIN);
      break;
    case Qt::Key_G:
      getApp()->toggleGodMode();
      break;
  }
}

void GameScreen::mouseMoveEvent (QMouseEvent* event) {
  //Since mouse tracking isn't enabled, we only receive these when at least one button is pressed
  if (event->buttons() & Qt::LeftButton) {
    inputManager->touchEventMove(event->x(), event->y());
  }
}

void GameScreen::mousePressEvent (QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    inputManager->touchEventDown(event->x(), event->y());
  }
}

void GameScreen::mouseReleaseEvent (QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    inputManager->touchEventUp(event->x(), event->y());
  }
}

void GameScreen::initializeGL () {
  getApp()->initGL(0, 0, 0, 0);
}

void GameScreen::startGame (int level) {
  makeCurrent(); //this kind of restore the openGL context
  getApp()->initGL(level, 0, 0, 0);
  getApp()->startGame(level);
}

void GameScreen::startMultiplayerGame (int level, bool server) {
  makeCurrent();
  getApp()->initGL(level, 0, 0, 0);
  if (server)
    getApp()->startServer();
  else
    getApp()->startClient("localhost"); //FIXME: no hardcoding :)
}

void GameScreen::resizeGL (int width, int height) {
  getApp()->resize(width, height);
}

void GameScreen::paintGL () {
  getApp()->simulate();
  getApp()->render();
}
