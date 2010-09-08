#include "MainWindow.h"

#include <QApplication>
#include <QMenuBar>
#include "DebugAction.h"
#include "jansson.h"

MainWindow::MainWindow(const char* serieJSON, const char* apkPath) 
  : QMainWindow(),
    menuScreen(new MenuScreen(this)),
    gameScreen(new GameScreen(this, serieJSON, apkPath)),
    menuInter(new InterMenu(this)),
    layout(new QStackedLayout) {
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

  debugOptions["Visibility"] = DEBUG_VISIBILITY;
  debugOptions["Waypoints"] = DEBUG_WAYPOINTS;
  debugOptions["AI"] = DEBUG_AI;
  debugOptions["Shadows"] = DEBUG_SHADOWS;
  debugOptions["Collisions"] = DEBUG_COLLISIONS;
  createActions();
  createMenus();

  restoreSettings();
}

MainWindow::~MainWindow () {
  delete layout;
  delete menuScreen;
  delete gameScreen;
}

void MainWindow::debugChanged (eDebug what, bool enabled) {
  LOGE("debugChanged : what=%i, enabled=%i", what, enabled);
  if (enabled)
    enableDebug(what);
  else
    disableDebug(what);
  settings.setValue(debugOptions.key(what), enabled);
}

void MainWindow::restoreSettings () {
  LOGE("Restoring settings from : %s", settings.fileName().toStdString().data());
  for (QHash<QString,eDebug>::iterator i=debugOptions.begin(); i!=debugOptions.end(); i++) {
    bool val = settings.value(i.key(), false).toBool();
    debugActions[i.key()]->setChecked(val);
  }
}

void MainWindow::createActions () {
  for (QHash<QString,eDebug>::iterator i=debugOptions.begin(); i!=debugOptions.end(); i++) {
    DebugAction* a = new DebugAction(i.key(), this, i.value());
    debugActions[i.key()] = a;
    connect(a, SIGNAL(debugChanged(eDebug, bool)), this, SLOT(debugChanged(eDebug, bool)));
  }
}

void MainWindow::createMenus () {
  QMenu* debugMenu = menuBar()->addMenu("Debug");
  for (QMap<QString, DebugAction*>::iterator i=debugActions.begin(); i!=debugActions.end(); i++) 
    debugMenu->addAction(i.value());
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

