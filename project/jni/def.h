#ifndef DEF_H_
#define DEF_H_

#include <stdlib.h>
#include <zip.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

//FIXME: this is to get nice code completion under netbeans... find another way to fix this
#ifndef size_t
#define size_t uint64_t
#endif

#define ASSERT(x) assert(x)


#define LOG_TAG    __FILE__ ":" STR(__LINE__)
#define STR(x) STRINGIFY(x)
#define STRINGIFY(x) # x


#ifdef PLATFORM_SDL //SDL standalone build
  #include <gl.h>
  #define LOGI(...) do {printf(__VA_ARGS__);printf("\n");} while(0)
  #define LOGE(...) do {printf(__VA_ARGS__);printf("\n");} while(0)
#else //Android build
  #include <GLES/gl.h>
  #include <android/log.h>
  #if 1
    #define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
    #define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
  #else
    #define LOGI(...)
    #define LOGE(...)
  #endif
#endif

//Handy macro to define unused parameters
//void dcc_mon_siginfo_handler(int UNUSED(whatsig))
//http://sourcefrog.net/weblog/software/languages/C/unused.html
#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

//BEGIN opengl-related defs
//Since the desktop version of opengl doesn't support fixed-point,
#ifdef PLATFORM_SDL
  #define MGL_DATATYPE float
  #define MGL_TYPE GL_FLOAT
  #define iX(x) (x)
  #define fX(x) (x)
#else
  #define MGL_DATATYPE int
  #define MGL_TYPE GL_FIXED
  //int to fixed point
  #define iX(x) (x<<16)
  //float to fixed point
  #define fX(x) ((int)(x * (1  << 16)))
#endif

//END

//logical tile size
#define TILE_SIZE 1.0
#define GRID_CELL_SIZE 0.5f
#define TANK_BCIRCLE_R 0.35f
//DONT use bcircle radius that would be multiple of cell size, this can create bugs with pathfinding and coldet because of rounding (pathfinding finds a path
//but the collision detection forbids it)
#define BOSS_BCIRCLE_R 0.45f
#define ROCKET_BCIRCLE_R 0.2f
#define BOMB_BCIRCLE_R 0.4f

#define ROCKET_MAX_BOUNCES 3

#define MAX_BOMBS_PER_TANK 3

#define BOMB_LIFETIME 10
#define BOMB_EXPLOSION_RADIUS 1.5f

//texture groups
enum {
    TEX_GROUP_UTILS=0, //always present
    TEX_GROUP_GAME, //game related textures
    TEX_GROUP_MENU, //main menu
    TEX_GROUP_TUTORIAL,
    TEX_GROUP_GET_FULL_VERSION,
    TEX_GROUP_REWARD
};

//network server state
enum ServerState {
  WARM_UP=0,
  ROUND_WARM_UP=1,
  IN_ROUND=2,
  GAME_FINISHED=3
};

#endif /* DEF_H_ */
