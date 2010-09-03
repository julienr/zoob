#ifndef ZOOB_INTER_MENU_H
#define ZOOB_INTER_MENU_H

#include <QWidget>
#include <QLabel>
class MainWindow;
class InterMenu : public QWidget {
  Q_OBJECT
  public:
    InterMenu (MainWindow* parent);

    void setText (const char* text);

    void mousePressEvent (QMouseEvent *event);

  private:
    MainWindow* window;
    QLabel* label;
};
#endif
