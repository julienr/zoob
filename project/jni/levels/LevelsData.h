#ifndef LEVELSDATA_H_
#define LEVELSDATA_H_

#include "logic/Level.h"

typedef Level* (*lvl_callback_t) ();

extern const size_t numLevels;
extern lvl_callback_t levelsLoadFns[];

#endif /* LEVELSDATA_H_ */
