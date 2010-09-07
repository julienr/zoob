#include "MainWindow.h"

#include <QApplication>
#include "jansson.h"

MainWindow::MainWindow(const char* serieJSON, const char* apkPath) 
  : menuScreen(new MenuScreen(this)),
    gameScreen(new GameScreen(this, serieJSON, apkPath)),
    menuInter(new InterMenu(this)),
    layout(new QStackedLayout) {
  setLayout(layout);

  layout->addWidget(menuScreen);
  layout->addWidget(gameScreen);
  layout->addWidget(menuInter);
  layout->setCurrentWidget(menuScreen);

  json_error_t error;
  json_t* json = json_loads(serieJSON, &error);
  if (!json) {
    LOGE("Error loading serie : %s (at line %i)", error.text, error.line);
  } else {
    json_t* levelsArr = json_object_get(json, "levels");
    menuScreen->setLevelsRange(0, json_array_size(levelsArr)-1);
    json_decref(levelsArr);
    json_decref(json);
  }
}

MainWindow::~MainWindow () {
  delete layout;
  delete menuScreen;
  delete gameScreen;
}

void MainWindow::startGame (int level) {
  layout->setCurrentWidget(gameScreen);
  if (level == -1)
    level = menuScreen->getSelectedLevel();
  gameScreen->startGame(level);
}

void MainWindow::showMenu (eMenu menu, int currentLevel) {
  switch (menu) {
    case MENU_MAIN:  
      layout->setCurrentWidget(menuScreen);
      break;
    case MENU_WON:
      menuInter->setText("You won");
      layout->setCurrentWidget(menuInter);
      if (currentLevel != -1)
        menuScreen->nextLevel();
      break;
    case MENU_LOST:
      menuInter->setText("You lost");
      layout->setCurrentWidget(menuInter);
      break;
    case MENU_END:
      menuInter->setText("End");
      layout->setCurrentWidget(menuInter);
      break;
    case MENU_ERROR:
      menuInter->setText("Loading error");
      layout->setCurrentWidget(menuInter);
      break;
    case MENU_LAST:
      LOGE("Error, request to show MENU_LAST");
      break;
  }
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
