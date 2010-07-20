#include "LevelsData.h"
#include <zip.h>
#include <jansson.h>

Level* levelFromJSON (json_t* json);

//The currently used levelSerie
json_t* levelSerie = NULL;

//get from an object, ensuring we don't get NULL
static json_t* obj_get (json_t* obj, const char* key) {
  json_t* val = json_object_get(obj, key);
  if (!val) {
    fprintf(stderr, "ERROR getting %s\n", key);
    //FIXME: no exit() on android, should goto error
    exit(-1);
  }
  return val;
}

static const char* type2str (json_type t) {
  switch (t) {
    case JSON_OBJECT: return "JSON_OBJECT";
    case JSON_ARRAY: return "JSON_ARRAY";
    case JSON_STRING: return "JSON_STRING";
    case JSON_INTEGER: return "JSON_INTEGER";
    case JSON_REAL: return "JSON_REAL";
    case JSON_TRUE: return "JSON_TRUE";
    case JSON_FALSE: return "JSON_FALSE";
    case JSON_NULL: return "JSON_NULL";
  }
}

//check that val is of the given type
static json_t* expect (json_t* val, json_type expected_type) {
  if (json_typeof(val) != expected_type) {
    fprintf(stderr, "val type(%s) != expected_type(%s)\n", type2str(json_typeof(val)), type2str(expected_type));
    //FIXME: no exit() on android, should goto error
    exit(-1);
  }
  return val;
}

static bool json_bool_value(json_t* val) {
  if (json_typeof(val) == JSON_TRUE)
    return true;
  if (json_typeof(val) == JSON_FALSE)
    return false;
  fprintf(stderr, "json_bool_value : not bool (%i)", json_typeof(val));
  //FIXME: no exit() on android, should goto error
  exit(-1);
}

static int str2tile (char c) {
  switch (c) {
    case 'W': return W;
    case 'T': return T;
    case 'E': return E;
    case 'R': return R;
    case 'L': return L;
    case 'B': return B;
    case 'M': return M;
    default: LOGE("str2tile, unknown character %c", c); return -1;
  }
}

#define TT_CASE(x, ret) if (strcmp(x, str) == 0) { return ret; }
static int str2ttype (const char* str) {
  TT_CASE("player", TANK_PLAYER)
  TT_CASE("static", TANK_STATIC)
  TT_CASE("simple", TANK_SIMPLE)
  TT_CASE("shield", TANK_SHIELD)
  TT_CASE("bounce", TANK_BOUNCE)
  TT_CASE("split", TANK_SPLIT)
  TT_CASE("burst", TANK_BURST)
  TT_CASE("boss_split", BOSS_SPLIT)
  TT_CASE("boss_simple", BOSS_SIMPLE)
  TT_CASE("boss_shield", BOSS_SHIELD)
  TT_CASE("boss_bounce", BOSS_BOUNCE)
  TT_CASE("boss_burst", BOSS_BURST)

  LOGE("str2ttype, unknown type %s", str);
  return -1;
}

#define CONDITION(x,err) if (!(x)) { LOGE(err); goto error; }

//Loads a level from the APK (an original level)
//Return NULL if an error occurs during loading
Level* loadAPKLevel (int levelNum) {
  ASSERT(levelSerie != NULL && levelNum < (int)getNumLevels());
  json_t* levels = obj_get(levelSerie, "levels");
  return levelFromJSON(json_array_get(levels, levelNum));
}

size_t getNumLevels () {
  ASSERT(levelSerie != NULL);
  json_t* levels = obj_get(levelSerie, "levels");
  return json_array_size(levels);
}

void loadSerie (const char* serieFile) {
  if (levelSerie != NULL) {
    json_decref(levelSerie);
    levelSerie = NULL;
  }

  zip_file* file = zip_fopen(APKArchive, serieFile, 0);

  struct zip_stat stats;
  if (zip_stat(APKArchive, serieFile, 0, &stats) == -1) {
    LOGE("Cannot zip_stat file %s", serieFile);
    //FIXME: error
    levelSerie = NULL;
    return;
  }

  char* buffer = (char*)malloc(sizeof(char)*(stats.size+1));

  if (zip_fread(file, buffer, stats.size) == -1) {
    LOGE("Error during zip_fread on %s", serieFile);
    //FIXME: error
    levelSerie = NULL;
    return;
  }

  buffer[stats.size] = '\0';

  json_error_t error;
  levelSerie = json_loads(buffer, &error);
  free(buffer);

  if (!levelSerie) {
    LOGE("Error loading json at line %i: %s", error.line, error.text);
    //FIXME: should goto error => create a global levelLoadError function that display the error menu
    levelSerie = NULL;
  }
}

//Create a Level object from a json representation of the level
Level* levelFromJSON (json_t* json) {
  CONDITION(json_typeof(json) == JSON_OBJECT, "root JSON element not an object");
  //GENERAL PROPERTIES
  {
    int xdim = json_integer_value(expect(obj_get(json, "xdim"), JSON_INTEGER));
    int ydim = json_integer_value(expect(obj_get(json, "ydim"), JSON_INTEGER));

    bool shadows = json_bool_value(obj_get(json, "shadows"));
    bool boss = json_bool_value(obj_get(json, "boss"));

    //TILES
    eTileType* board = new eTileType[xdim*ydim];

    json_t* tiles_arr = obj_get(json, "tiles");
    CONDITION(tiles_arr && json_typeof(tiles_arr) == JSON_ARRAY, "tiles element not an array");
    CONDITION((int)json_array_size(tiles_arr) == ydim, "tiles array size != ydim");
    for (int y=0; y<ydim; y++) {
      json_t* row = json_array_get(tiles_arr, y);
      CONDITION((int)json_array_size(row) == xdim, "tiles row size != xdim");
      for (int x=0; x<xdim; x++) {
        json_t* val = json_array_get(row, x);
        CONDITION(json_typeof(val) == JSON_STRING, "tile element type not a string");
        const char* t = json_string_value(val);
        int ttype = str2tile(t[0]);
        if (ttype == -1)
          goto error;
        board[y*xdim+x] = (eTileType)ttype;
      }
    }

    //TANKS
    json_t* tanks_arr = obj_get(json, "tanks");
    CONDITION(tanks_arr && json_typeof(tanks_arr) == JSON_ARRAY, "tanks element not an array");

    const int numTanks = json_array_size(tanks_arr);
    TankDescription* tanks = new TankDescription[numTanks];

    for (int i=0; i<numTanks; i++) {
      json_t* tank_obj = json_array_get(tanks_arr, i);
      CONDITION(json_typeof(tank_obj) == JSON_OBJECT, "tank element not an object");
      json_t* coords_obj = obj_get(tank_obj, "coords");
      CONDITION(json_typeof(coords_obj) == JSON_ARRAY && json_array_size(coords_obj) == 2, "tank coord should be an array of size 2");
      int coords[2];
      coords[0] = json_integer_value(expect(json_array_get(coords_obj, 0), JSON_INTEGER));
      coords[1] = json_integer_value(expect(json_array_get(coords_obj, 1), JSON_INTEGER));

      const char* type = json_string_value(expect(obj_get(tank_obj, "type"), JSON_STRING));
      int tmp = str2ttype(type);
      if (tmp == -1)
        goto error;
      eTankType ttype = (eTankType)tmp;

      Path* path;
      //Test for path
      json_t* path_arr = json_object_get(tank_obj, "path");
      if (path_arr) {
        CONDITION(json_typeof(path_arr) == JSON_ARRAY, "path element should be an array");

        const int pathSize = json_array_size(path_arr);
        Vector2* pathWp = new Vector2[pathSize];
        for (int j=0; j<pathSize; j++) {
          json_t* wp = json_array_get(path_arr, j);
          CONDITION(json_typeof(wp) == JSON_ARRAY && json_array_size(wp) == 2, "path waypoint element should be arrays of size 2");
          int x = json_integer_value(expect(json_array_get(wp,0),JSON_INTEGER));
          int y = json_integer_value(expect(json_array_get(wp,1),JSON_INTEGER));
          pathWp[j] = Vector2(x,y);
        }
        path = new Path(pathSize, pathWp);
      } else {
        path = NULL;
      }
      tanks[i] = TankDescription(coords[0], coords[1], ttype, path);
    }
    Level* l = new Level(xdim, ydim, board, tanks, numTanks, shadows, boss);
    delete[] board;
    return l;
  }
error:
  return NULL;
}
