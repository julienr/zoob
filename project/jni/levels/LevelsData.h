#ifndef LEVELSDATA_H_
#define LEVELSDATA_H_

#include "logic/Level.h"

//Loads a given level from the serie
Level* loadAPKLevel (int levelNum);

//Returns a given level in its text representation
//The returned pointer MUST be freed by the caller
char* getLevel (int levelNum);

//Change the currently active serie
void loadSerie(const char* serieJSON);

//Get number of levels in the current serie
size_t getNumLevels ();

#endif /* LEVELSDATA_H_ */
