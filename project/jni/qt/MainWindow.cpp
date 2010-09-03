#include "MainWindow.h"

#include <QApplication>

MainWindow::MainWindow(const char* serieJSON, const char* apkPath) 
  : menuScreen(new MenuScreen(this)),
    gameScreen(new GameScreen(this, serieJSON, apkPath)),
    layout(new QStackedLayout) {
  setLayout(layout);

  layout->addWidget(menuScreen);
  layout->addWidget(gameScreen);
}

MainWindow::~MainWindow () {
  delete layout;
  delete menuScreen;
  delete gameScreen;
}

void MainWindow::startGame () {
  layout->setCurrentWidget(gameScreen);
  gameScreen->startGame(2);
}

void MainWindow::showMenu () {
  layout->setCurrentWidget(menuScreen);
}

#define PROCESS_EVENTS_INTERVAL 50
void MainWindow::mainLoop () {
  running = true;
  timer.start();

  QPainter painter;
  while (running) {
    if (timer.elapsed() > PROCESS_EVENTS_INTERVAL) {
      timer.start();
      QApplication::processEvents();
    }
    if (layout->currentWidget() == gameScreen) {
      gameScreen->repaint();
    }
  }
}

void MainWindow::quit () {
  running = false;
}
