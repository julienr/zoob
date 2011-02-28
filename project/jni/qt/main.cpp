#include <QApplication>
#include <QTimer>
#include <QObject>
#include <QMap>
#include <unistd.h>
#include <sys/stat.h>

#include "MainWindow.h"

#include "zoobdef.h"
#include "app.h"
#include "input/AndroidInputManager.h"
#include "lib/FileManager.h"


static MainWindow* window = NULL;

class AppQT : public AppInterface {
  public:
    AppQT (const char* resourcesDir, const char* serie)
      : AppInterface(new FSFileManager(resourcesDir), serie) {}

    virtual InputManager* createInputManager (bool useGamepad, bool useTrackball);

    virtual void saveProgress (int level);

    virtual void showMenu (eMenu id, int currentLevel);
};

void AppQT::saveProgress (int level) {
  LOGE("saveProgress : %i", level);
}

void AppQT::showMenu (eMenu id, int currentLevel) {
  LOGE("showMenu (%i) with currentLevel=%i", id, currentLevel);
  window->showMenu((eMenu)id, currentLevel);
}

InputManager* AppQT::createInputManager (bool useGamepad, bool useTrackball) {
  return window->createInputManager(useGamepad, useTrackball);
}

char* loadJSON (const char* serieFile) {
  FILE* file = fopen(serieFile, "r");

  if (!file) {
    LOGE("Cannot open file %s", serieFile);
    exit(-1);
  }

  struct stat stats;
  if (stat(serieFile, &stats) == -1) {
    LOGE("Cannot zip_stat file %s", serieFile);
    exit(-1);
  }

  char* buffer = (char*)malloc(sizeof(char)*(stats.st_size+1));

  if (fread(buffer, 1, stats.st_size, file) == 0) {
    LOGE("Error reading %s", serieFile);
    exit(-1);
  }

  buffer[stats.st_size] = '\0';
  fclose(file);
  return buffer;
}


static AppQT* appInterface = NULL;

AppInterface* getApp () {
  return appInterface;
}


int main (int argc, char** argv) {
  bool hasArgs = true;
  if (argc < 3) {
    LOGE("usage : <json file> <apk file> [lvlnum]");
    LOGE("falling back on default arguments");
    hasArgs = false;
    //exit(-1);
  }

  char* json = loadJSON(hasArgs?argv[1]:"../../../levels/test2.json");

  QApplication app(argc, argv);

  //Some default values (used by settings)
  QCoreApplication::setOrganizationName("fhtagn");
  QCoreApplication::setOrganizationDomain("zoob.fhtagn.net");
  QCoreApplication::setApplicationName("Zoob");

  const char* resourcesDir = hasArgs?argv[2]:"../../";


  appInterface = new AppQT(resourcesDir, json);
  //Main window creation
  window = new MainWindow();
  free(json);
  window->resize(640, 480);
  window->show();

  //Parse optional arguments :
  //-l1 specify the level
  //-s specify start as network server
  //-c specify start as network client
  bool immediateStart = false;
  int opt;
  while ((opt = getopt(argc, argv, "l:sc")) != -1) {
    switch (opt) {
      case 'l': {
        int level = atoi(optarg);
        window->selectLevel(level);
        immediateStart = true;
        break;
      }
      case 's': {
        window->setServer();
        immediateStart = true;
        break;
      }
      case 'c': {
        window->setClient();
        immediateStart = true;
        break;
      }
    }
  }

  //Fire main loop with a one time timer just after QApplication start
  QTimer::singleShot(0, window, SLOT(mainLoop()));
  QObject::connect(&app, SIGNAL(lastWindowClosed()), window, SLOT(quit()));

  //If we should start immediatly, just let QT initialize and then startGame
  if (immediateStart) {
    QTimer::singleShot(100, window, SLOT(startGame()));
  }
  app.exec();

  //cleanup
  delete window;
}
