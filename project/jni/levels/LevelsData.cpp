#include "LevelsData.h"

eTileType coltest[3*2] = {
    E,E,E,
    E,E,W
};

Level* loadColTest() {
  return new Level(3,2,&coltest[0]);
}

eTileType lvl1[7*8] = {
    W, W, W, W, W, W, W,
    W, E, E, E, E, E, W,
    W, S, E, W, E, E, W,
    W, E, E, E, W, E, W,
    W, E, E, E, W, E, W,
    W, W, W, E, W, E, W,
    W, E, E, E, W, E, W,
    W, W, W, W, W, W, W,
};

Level* loadLevel1() {
  return new Level(7, 8, &lvl1[0]);
}
