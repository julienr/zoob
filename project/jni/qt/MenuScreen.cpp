#include "MenuScreen.h"
#include "MainWindow.h"

#include <QPushButton>
#include <QVBoxLayout>

MenuScreen::MenuScreen(MainWindow* window) : QWidget(window) {
  QPushButton* btn = new QPushButton("test");

  connect(btn, SIGNAL(clicked()), window, SLOT(startGame()));


  QVBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(btn);
  setLayout(layout);
}
