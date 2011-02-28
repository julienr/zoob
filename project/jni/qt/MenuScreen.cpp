#include "MenuScreen.h"
#include "MainWindow.h"

#include <QLabel>

MenuScreen::MenuScreen(MainWindow* window) : QWidget(window) {
  playBtn = new QPushButton("play");
  playBtn->setDefault(true);
  playBtn->setFocus();
  connect(playBtn, SIGNAL(clicked()), window, SLOT(startGame()));

  //Gametype radio buttons
  QVBoxLayout *vbox = new QVBoxLayout();
  QRadioButton* btn1 = new QRadioButton("local");
  QRadioButton* btn2 = new QRadioButton("server");
  QRadioButton* btn3 = new QRadioButton("client");
  gameType = new QButtonGroup();
  gameType->addButton(btn1, LOCAL);
  vbox->addWidget(btn1);
  gameType->addButton(btn2, SERVER);
  vbox->addWidget(btn2);
  gameType->addButton(btn3, CLIENT);
  vbox->addWidget(btn3);
  vbox->addStretch(1);

  QGroupBox* groupBox = new QGroupBox("Game type");
  groupBox->setLayout(vbox);

  connect(gameType, SIGNAL(buttonClicked(int)), this, SLOT(gameTypeChanged(int))); 

  //Gametype options
  QVBoxLayout *options = new QVBoxLayout;
  
  //[LOCAL, SERVER] level selection
  QHBoxLayout* lvllayout = new QHBoxLayout;
  levelsSpin = new QSpinBox();
  lvllayout->addWidget(new QLabel("Level : "));
  lvllayout->addWidget(levelsSpin);
  lvllayout->addStretch(1);

  levelOption = new QWidget();
  levelOption->setLayout(lvllayout);
  options->addWidget(levelOption);

  //[CLIENT] server ip address
  QHBoxLayout* addrlayout = new QHBoxLayout;
  serverIP = new QLineEdit();
  serverIP->setText(settings.value("server_ip", "localhost").toString());
  addrlayout->addWidget(new QLabel("Server IP : "));
  addrlayout->addWidget(serverIP);
  addrlayout->addStretch(1);


  ipOption = new QWidget();
  ipOption->setLayout(addrlayout);
  options->addWidget(ipOption);


  //Default to LOCAL gameType
  gameType->button(LOCAL)->setChecked(true);
  gameTypeChanged(LOCAL);

  QGroupBox* optionsBox = new QGroupBox("Options");
  optionsBox->setLayout(options);

  layout = new QVBoxLayout;
  layout->addWidget(groupBox);
  layout->addWidget(optionsBox);
  layout->addWidget(playBtn);
  layout->addStretch(1);
  setLayout(layout);
}

MenuScreen::~MenuScreen () {
  settings.setValue("server_ip", serverIP->text());
  delete playBtn;
  delete layout;
  delete levelsSpin;
  delete serverIP;
}

void MenuScreen::gameTypeChanged (int id) {
  switch (id) {
    case LOCAL:
    case SERVER:
      levelOption->setVisible(true);
      ipOption->setVisible(false);
      break;
    case CLIENT:
      levelOption->setVisible(false);
      ipOption->setVisible(true);
      break;
    default:
      LOGE("Error unhandled id : %i\n", id);
      break;
  }
  LOGE("id : %i\n", id);
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
  return (eGameType)gameType->checkedId();
}
