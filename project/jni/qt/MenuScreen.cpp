#include "MenuScreen.h"
#include "MainWindow.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

MenuScreen::MenuScreen(MainWindow* window) : QWidget(window) {
  QPushButton* playBtn = new QPushButton("play");
  playBtn->setDefault(true);
  playBtn->setFocus();
  connect(playBtn, SIGNAL(clicked()), window, SLOT(startGame()));

  levelsSpin = new QSpinBox();
  gameType = new QComboBox();
  gameType->addItem("local", LOCAL);
  gameType->addItem("server", SERVER);
  gameType->addItem("client", CLIENT);

  QVBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(gameType);
  layout->addWidget(levelsSpin);
  layout->addWidget(playBtn);
  setLayout(layout);
}

void MenuScreen::setLevelsRange (int min, int max) {
  levelsSpin->setRange(min, max);
  levelsSpin->setSingleStep(1);
}

void MenuScreen::selectLevel (int level) {
  levelsSpin->setValue(level);
}

void MenuScreen::nextLevel () {
  levelsSpin->setValue(levelsSpin->value()+1);
}


int MenuScreen::getSelectedLevel () {
  return levelsSpin->value();
}

eGameType MenuScreen
::getGameType() {
  return (eGameType)gameType->itemData(gameType->currentIndex()).toInt();
}
