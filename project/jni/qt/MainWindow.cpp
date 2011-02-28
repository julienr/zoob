#include "MainWindow.h"

#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>
#include "DebugAction.h"
#include "jansson.h"

MainWindow::MainWindow(const char* serieJSON) 
  : QMainWindow(),
    menuScreen(new MenuScreen(this)),
    gameScreen(new GameScreen(this, serieJSON)),
    menuInter(new InterMenu(this)),
    layout(new QStackedLayout),
    statusBar(new QStatusBar) {
  setStatusBar(statusBar);

  QWidget* container = new QWidget();
  container->setLayout(layout);
  this->setCentralWidget(container);

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

  debugOptions["Visibility"] = AppInterface::DEBUG_VISIBILITY;
  debugOptions["AI"] = AppInterface::DEBUG_AI;
  debugOptions["Shadows"] = AppInterface::DEBUG_SHADOWS;
  debugOptions["Collisions"] = AppInterface::DEBUG_COLLISIONS;
  debugOptions["Overlays"] = AppInterface::DEBUG_OVERLAYS;
  createActions();
  createMenus();

  restoreSettings();
}

MainWindow::~MainWindow () {
  delete layout;
  delete menuScreen;
  delete menuInter;
  delete gameScreen;
  delete statusBar;
}

void MainWindow::debugChanged (AppInterface::eDebug what, bool enabled) {
  LOGE("debugChanged : what=%i, enabled=%i", what, enabled);
  if (enabled)
    getApp()->enableDebug(what);
  else
    getApp()->disableDebug(what);
  settings.setValue(debugOptions.key(what), enabled);
}

void MainWindow::restoreSettings () {
  LOGE("Restoring settings from : %s", settings.fileName().toStdString().data());
  for (QHash<QString,AppInterface::eDebug>::iterator i=debugOptions.begin(); i!=debugOptions.end(); i++) {
    bool val = settings.value(i.key(), false).toBool();
    debugActions[i.key()]->setChecked(val);
  }
}

void MainWindow::createActions () {
  for (QHash<QString,AppInterface::eDebug>::iterator i=debugOptions.begin(); i!=debugOptions.end(); i++) {
    DebugAction* a = new DebugAction(i.key(), this, i.value());
    debugActions[i.key()] = a;
    connect(a, SIGNAL(debugChanged(AppInterface::eDebug, bool)), this, SLOT(debugChanged(AppInterface::eDebug, bool)));
  }
}

void MainWindow::createMenus () {
  QMenu* debugMenu = menuBar()->addMenu("Debug");
  for (QMap<QString, DebugAction*>::iterator i=debugActions.begin(); i!=debugActions.end(); i++) 
    debugMenu->addAction(i.value());
}

void MainWindow::startGame () {
  layout->setCurrentWidget(gameScreen);
  int level = menuScreen->getSelectedLevel();
  eGameType type = menuScreen->getGameType();
  switch (type) {
    case LOCAL:
      setStatusTip("local game");
      gameScreen->startGame(level);
      break;
    case SERVER:
      setStatusTip("server");
      gameScreen->startMultiplayerGame(level, true);
      break;
    case CLIENT:
      setStatusTip("client");
      gameScreen->startMultiplayerGame(level, false);
      break;
  }
}

void MainWindow::showMenu (AppInterface::eMenu menu, int currentLevel) {
  switch (menu) {
    case AppInterface::MENU_MAIN:  
      layout->setCurrentWidget(menuScreen);
      break;
    case AppInterface::MENU_WON:
      menuInter->setText("You won");
      layout->setCurrentWidget(menuInter);
      if (currentLevel != -1)
        menuScreen->nextLevel();
      break;
    case AppInterface::MENU_LOST:
      menuInter->setText("You lost");
      layout->setCurrentWidget(menuInter);
      break;
    case AppInterface::MENU_END:
      menuInter->setText("End");
      layout->setCurrentWidget(menuInter);
      break;
    case AppInterface::MENU_ERROR:
      menuInter->setText("Loading error");
      layout->setCurrentWidget(menuInter);
      break;
    case AppInterface::MENU_LAST:
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

