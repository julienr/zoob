#include "LevelsData.h"

eTileType coltest[3*5] = {
    S,E,E,
    E,E,W,
    S,E,W,
    E,E,W,
    E,W,W
};

Level* loadColTest() {
  return new Level(3,5,&coltest[0]);
}

eTileType empty[7*7] = {
    E,E,E,E,E,E,E,
    E,S,E,E,E,E,E,
    E,E,E,E,E,E,E,
    E,E,E,E,S,E,E,
    E,E,E,E,E,E,E,
    E,E,E,E,E,E,E,
    E,E,E,E,E,E,E
};

Level* loadEmpty() {
  return new Level(7,7,&empty[0]);
}

eTileType lvl1[7*8] = {
    W, W, W, W, W, W, W,
    W, E, E, E, E, E, W,
    W, S, E, S, E, E, W,
    W, E, E, E, W, E, W,
    W, E, E, E, W, E, W,
    W, W, W, E, W, E, W,
    W, E, E, E, W, E, W,
    W, W, W, W, W, W, W,
};

Level* loadLevel1() {
  return new Level(7, 8, &lvl1[0]);
}

eTileType lvl2[8*8] = {
    W, T, T, T, T, T, T, W,
    L, E, E, E, E, E, E, R,
    L, S, E, S, E, E, E, R,
    L, E, E, E, R, E, E, R,
    L, E, T, T, W, E, E, R,
    L, E, E, E, E, E, E, R,
    L, E, E, E, E, E, E, R,
    W, B, B, B, B, B, B, W
};

Level* loadLevel2() {
  return new Level(8, 8, &lvl2[0]);
}

//Array fill
const size_t numLevels = 4;
lvl_callback_t levelsLoadFns[numLevels] = {
  &loadColTest,
  &loadEmpty,
  &loadLevel1,
  &loadLevel2
};
