#include "InterMenu.h"
#include <QLabel>
#include <QVBoxLayout>

#include "MainWindow.h"

InterMenu::InterMenu(MainWindow* parent) :
  QWidget(parent),
  window(parent) {
  label = new QLabel("");
  label->setAlignment(Qt::AlignCenter);

  QLabel* contLabel = new QLabel("click to continue");
  contLabel->setAlignment(Qt::AlignCenter);

  QVBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(label);
  layout->addWidget(contLabel);
  setLayout(layout);
}

void InterMenu::setText(const char* text) {
  label->setText(text);
}

void InterMenu::mousePressEvent (QMouseEvent* /*event*/) {
  window->showMenu(AppInterface::MENU_MAIN);
}
