#ifndef ZOOB_MENU_SCREEN_H
#define ZOOB_MENU_SCREEN_H

#include <QWidget>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QRadioButton>
#include <QGroupBox>
#include <QSettings>
#include <QLineEdit>

class MainWindow;

enum eGameType {
  LOCAL=0,
  SERVER=1,
  CLIENT=2
};

class MenuScreen : public QWidget {
  Q_OBJECT
  public:
    MenuScreen (MainWindow* parent);

    virtual ~MenuScreen ();

    void setLevelsRange (int min, int max);

    int getSelectedLevel ();
    const char* getServerAddress () const {
      return serverIP->text().toAscii();
    }
    eGameType getGameType();

    void setGameType (eGameType t) {
      gameType->button(t)->setChecked(true);
    }

    void selectLevel (int level);
    void nextLevel ();
  public slots:
    void gameTypeChanged (int id);

  private:
    QSettings settings;
    QSpinBox* levelsSpin;
    QWidget* levelOption;
    QLineEdit* serverIP;
    QWidget* ipOption;
    QPushButton* playBtn;
    QVBoxLayout* layout;
    QButtonGroup* gameType;
};

#endif
