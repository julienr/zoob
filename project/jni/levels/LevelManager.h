#ifndef LEVEL_MANAGER_H_
#define LEVEL_MANAGER_H_

#include "logic/Level.h"
#include <jansson.h>

class LevelManager {
  public:
    static void registerInstance (LevelManager* inst) {
      ASSERT(!instance);
      instance = inst;
    }

    static LevelManager* getInstance() {
      return instance;
    }

    virtual ~LevelManager () {}

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
    static LevelManager* instance;
    bool error;
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

#endif /* LEVEL_MANAGER_H_ */
