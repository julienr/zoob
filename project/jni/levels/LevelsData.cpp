#include "LevelsData.h"

eTileType lvl1[8][7] = {
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
  //FIXME: why the fuck do we need a cast ?
  return new Level(7, 8, &lvl1[0][0]);
}
