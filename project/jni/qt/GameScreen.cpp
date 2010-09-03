#include "GameScreen.h"
#include "MainWindow.h"
#include "app.h"

GameScreen::GameScreen (MainWindow* parent, const char* json, const char* apkPath) :
  QGLWidget(QGLFormat(QGL::DoubleBuffer |
                      QGL::DepthBuffer |
                      QGL::Rgba |
                      QGL::AlphaChannel |
                      QGL::AccumBuffer |
                      QGL::StencilBuffer |
                      QGL::DirectRendering |
                      QGL::NoSampleBuffers)),
  mainWindow(parent) {
  nativeInit(apkPath, json);
  nativeLoadSerie(json);
}

void GameScreen::keyPressEvent (QKeyEvent* event) {
  event->accept();
  switch (event->key()) {
    case Qt::Key_Escape:
      mainWindow->showMenu(MENU_MAIN);
      break;
  }
}

void GameScreen::mouseMoveEvent (QMouseEvent* event) {
  //Since mouse tracking isn't enabled, we only receive these when at least one button is pressed
  if (event->buttons() & Qt::LeftButton) {
    LOGE("mouse move (%i,%i)", event->x(), event->y());
    inputManager->touchEventMove(event->x(), event->y());
  }
}

void GameScreen::mousePressEvent (QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    LOGE("touch event down");
    inputManager->touchEventDown(event->x(), event->y());
  }
}

void GameScreen::mouseReleaseEvent (QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    LOGE("touch event up");
    inputManager->touchEventUp(event->x(), event->y());
  }
}

void GameScreen::initializeGL () {
  nativeInitGL(0, 0, 0, 0);
}

void GameScreen::startGame (int level) {
  makeCurrent(); //this kind of restore the openGL context
  nativeInitGL(level, 0, 0, 0);
  ::startGame(level);
}

void GameScreen::resizeGL (int width, int height) {
  nativeResize(width, height);
}

void GameScreen::paintGL () {
  nativeRender();
}
