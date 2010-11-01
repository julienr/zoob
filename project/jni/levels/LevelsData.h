#ifndef LEVELSDATA_H_
#define LEVELSDATA_H_

#include "logic/Level.h"
#include <jansson.h>

class LevelManager {
  public:
    static void registerInstance(LevelManager* inst) {
      if (instance) {
        delete instance; 
        LOGI("[LevelManager::registerInstance] replacing current instance");
      }
      instance = inst;
    }

    static LevelManager* getInstance () {
      if (!instance)
        LOGE("[LevelManager::getInstance] called before registerInstance");
      return instance;
    }

    virtual Level* loadLevel (int levelNum) const = 0;
    virtual size_t getNumLevels () const = 0;

    virtual char* getLevel (int levelNum) const = 0;

    bool hasError () {
      return error;
    }

  protected:
    LevelManager () : error(false) {}

    void setError () {
      error = true;
    }

  private:
    bool error;
    static LevelManager* instance;
};

class SingleLevelManager : public LevelManager {
  public:
    SingleLevelManager (const char* levelJSON);

    Level* loadLevel (int levelNum) const;
    size_t getNumLevels () const { return 1; }

    char* getLevel (int levelNum) const;
  private:
    json_t* level;
};

class LevelSerieManager : public LevelManager {
  public:
    LevelSerieManager (const char* serieJSON);

    Level* loadLevel (int levelNum) const;
    size_t getNumLevels () const { return numLevels; }

    char* getLevel (int levelNum) const;
  private:
    json_t* serie;
    size_t numLevels;
};

//Loads a given level from the serie
/*Level* loadAPKLevel (int levelNum);

//Returns a given level in its text representation
//The returned pointer MUST be freed by the caller
char* getLevel (int levelNum);

//Change the currently active serie
void loadSerie(const char* serieJSON);

//Get number of levels in the current serie
size_t getNumLevels ();*/

#endif /* LEVELSDATA_H_ */
