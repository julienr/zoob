#include "LevelsData.h"
#include <zip.h>
#include <jansson.h>
#include "logic/Level.h"
#include "logic/Trigger.h"
#include "logic/triggers/Condition.h"
#include "logic/triggers/ConditionBreak.h"
#include "logic/triggers/Action.h"
#include "logic/triggers/ActionSpawn.h"

Level* levelFromJSON (json_t* json);

//This will be set to true if a loading error occured in one of this functions. This is checked by levelFromJSON
static bool loadError = false;

//get from an object, ensuring we don't get NULL
static json_t* obj_get (json_t* obj, const char* key) {
  json_t* val = json_object_get(obj, key);
  if (!val) {
    LOGE("ERROR getting %s", key);
    loadError = true;
    return NULL;
  }
  return val;
}

//The currently used levelSerie
json_t* levelSerie = NULL;

LevelManager* LevelManager::instance = NULL;

SingleLevelManager::SingleLevelManager (const char* levelJSON) : LevelManager () {
  json_error_t error;
  level = json_loads(levelJSON, &error);
  if (!level) {
    LOGE("Error loading json at line %i: %s", error.line, error.text);
    setError();
  } 
}

Level* SingleLevelManager::loadLevel (int levelNum) const {
  if (levelNum != 0) 
    LOGI("[SingleLevelManager::loadLevel] requested level %i, returning level 0", levelNum);
  return levelFromJSON(level);
}

char* SingleLevelManager::getLevel (int levelNum) const {
  return json_dumps(level, JSON_COMPACT);
}

LevelSerieManager::LevelSerieManager (const char* serieJSON) : LevelManager () {
  json_error_t error;
  serie = json_loads(serieJSON, &error);
  if (!serie) {
    LOGE("Error loading json at line %i: %s", error.line, error.text);
    setError();
  } else {
    json_t* levels = obj_get(serie, "levels");
    if (levels == NULL)
      setError();
    numLevels = json_array_size(levels);
  }
}

Level* LevelSerieManager::loadLevel (int levelNum) const {
  ASSERT(serie != NULL && levelNum < (int)getNumLevels());
  json_t* levels = obj_get(serie, "levels");
  return levelFromJSON(json_array_get(levels, levelNum));
}

char* LevelSerieManager::getLevel (int levelNum) const {
  ASSERT(serie != NULL && levelNum < (int)getNumLevels());
  json_t* levels = obj_get(serie, "levels");
  return json_dumps(json_array_get(levels, levelNum), JSON_COMPACT);
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
  if (loadError || val == NULL || (json_typeof(val) != expected_type)) {
    if (val != NULL)
      LOGE("val type(%s) != expected_type(%s)", type2str(json_typeof(val)), type2str(expected_type));
    else
      LOGE("val = NULL");
    loadError = true;
  }
  return val;
}

//if val != NULL, returns json_string_value, otherwise sets loadError = true 
static const char* json2string (json_t* val) {
  if (loadError || val == NULL) {
    LOGE("json2string : val = NULL");
    loadError = true;
    return NULL;
  } else {
    return json_string_value(val);
  }
}

static bool json_bool_value(json_t* val) {
  if (json_typeof(val) == JSON_TRUE)
    return true;
  if (json_typeof(val) == JSON_FALSE)
    return false;
  if (json_typeof(val) == JSON_INTEGER)
    return json_integer_value(val) != 0;
  LOGE("json_bool_value : not bool (%i)", json_typeof(val));
  loadError=true;
  return false;
}

//return true if the key is set to true, false otherwise (if not set or set to false)
static bool opt_istrue (json_t* obj, const char* key) {
  json_t* val = json_object_get(obj, key);
  if (!val || !json_bool_value(val))
    return false;
  else
    return true;
}

#define TT_CASE(x, ret) if (strcmp(x, str) == 0) { return ret; }
static int str2tile (const char* str) {
  TT_CASE("W", W);
  TT_CASE("T", T);
  TT_CASE("E", E);
  TT_CASE("R", R);
  TT_CASE("L", L);
  TT_CASE("B", B);
  TT_CASE("M", M);
  TT_CASE("TL", TL);
  TT_CASE("TR", TR);
  TT_CASE("BR", BR);
  TT_CASE("BL", BL);

  LOGE("str2tile, unknown tile %s", str); return -1;
  return -1;
}


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
/*Level* loadAPKLevel (int levelNum) {
  ASSERT(levelSerie != NULL && levelNum < (int)getNumLevels());
  json_t* levels = obj_get(levelSerie, "levels");
  return levelFromJSON(json_array_get(levels, levelNum));
}

char* getLevel (int levelNum) {
  ASSERT(levelSerie != NULL && levelNum < (int)getNumLevels());
  json_t* levels = obj_get(levelSerie, "levels");
  return json_dumps(json_array_get(levels, levelNum), JSON_COMPACT);
}

size_t getNumLevels () {
  ASSERT(levelSerie != NULL);
  json_t* levels = obj_get(levelSerie, "levels");
  return json_array_size(levels);
}

void loadSerie (const char* serieJSON) {
  //FIXME: if serie JSON is invalid, the program just crash
  if (levelSerie != NULL) {
    json_decref(levelSerie);
    levelSerie = NULL;
  }

  json_error_t error;
  levelSerie = json_loads(serieJSON, &error);
  //free(buffer);

  if (!levelSerie) {
    LOGE("Error loading json at line %i: %s", error.line, error.text);
    //FIXME: should goto error => create a global levelLoadError function that display the error menu
    levelSerie = NULL;
  }
}*/

#define RETONERR if (loadError) { return NULL; }

#define IF_STR_EQ(str, cond) if(strcmp(str, cond)==0)
static Condition* createCondition (const char* condStr) {
  IF_STR_EQ(condStr, "break") {
    return new ConditionBreak();
  } else {
    LOGE("Unknown condition %s", condStr);
    return NULL;
  }
}

static Action* createAction (const char* type, json_t* actionJSON) {
  IF_STR_EQ(type, "spawn_enemy") {
    const int dx = json_integer_value(expect(obj_get(actionJSON, "dx"), JSON_INTEGER)); RETONERR;
    const int dy = json_integer_value(expect(obj_get(actionJSON, "dy"), JSON_INTEGER)); RETONERR;
    return new ActionSpawn(dx, dy);
  }
  return NULL;
}

Trigger* handle_trigger(json_t* tileObj) {
  json_t* trigger = json_object_get(tileObj, "trigger");
  if (trigger == NULL || json_typeof(trigger) != JSON_OBJECT)
    return NULL;
  const char* conditionStr = json2string(expect(obj_get(trigger, "condition"), JSON_STRING)); RETONERR;
  json_t* action = expect(obj_get(trigger, "action"), JSON_OBJECT); RETONERR;
  const char* actionType = json2string(expect(obj_get(action, "type"), JSON_STRING)); RETONERR;

  LOGE("condition=%s, action=%s", conditionStr, actionType);

  Condition* cond = createCondition(conditionStr); RETONERR;
  Action* act = createAction(actionType, action); RETONERR;

  return new Trigger(act, cond); 
}

#define ERRCHK if (loadError) { goto error; }

//Create a Level object from a json representation of the level
Level* levelFromJSON (json_t* json) {
  CONDITION(json_typeof(json) == JSON_OBJECT, "root JSON element not an object");
  //GENERAL PROPERTIES
  {
    int xdim = json_integer_value(expect(obj_get(json, "xdim"), JSON_INTEGER)); ERRCHK;
    int ydim = json_integer_value(expect(obj_get(json, "ydim"), JSON_INTEGER)); ERRCHK;

    bool shadows = json_bool_value(obj_get(json, "shadows")); ERRCHK;
    bool boss = json_bool_value(obj_get(json, "boss")); ERRCHK;

    //TILES
    eTileType* board = new eTileType[xdim*ydim];
    bool* breakable = new bool[xdim*ydim];
    memset(breakable, 0, sizeof(bool)*xdim*ydim);

    list<TriggerDesc> triggers;

    json_t* tiles_arr = obj_get(json, "tiles"); ERRCHK;
    CONDITION(tiles_arr && json_typeof(tiles_arr) == JSON_ARRAY, "tiles element not an array");
    CONDITION((int)json_array_size(tiles_arr) == ydim, "tiles array size != ydim");
    for (int y=0; y<ydim; y++) {
      json_t* row = json_array_get(tiles_arr, y);
      CONDITION((int)json_array_size(row) == xdim, "tiles row size != xdim");
      for (int x=0; x<xdim; x++) {
        const char* t = "";
        json_t* val = json_array_get(row, x);
        //Basic tiles have the type directly
        //Advanced tiles might have options (like breakable or triggers) and be therefore an object instead of a string
        if (json_typeof(val) == JSON_STRING) {
          t = json_string_value(val);
        } else if (json_typeof(val) == JSON_OBJECT) {
          t = json_string_value(expect(obj_get(val, "type"), JSON_STRING)); ERRCHK;
          //breakable tiles
          const bool brk = opt_istrue(val, "brk"); //breakable
          breakable[y*xdim+x] = brk;
          
          //trigger
          const Trigger* trigger = handle_trigger(val); ERRCHK;
          if (trigger != NULL)
            triggers.append(TriggerDesc(trigger, x, y));
        }
        int ttype = str2tile(t);
        if (ttype == -1)
          goto error;
        board[y*xdim+x] = (eTileType)ttype;

      }
    }

    //TANKS
    json_t* tanks_arr = obj_get(json, "tanks"); ERRCHK;
    CONDITION(tanks_arr && json_typeof(tanks_arr) == JSON_ARRAY, "tanks element not an array");

    const int numTanks = json_array_size(tanks_arr);
    TankDescription* tanks = new TankDescription[numTanks];

    for (int i=0; i<numTanks; i++) {
      json_t* tank_obj = json_array_get(tanks_arr, i);
      CONDITION(json_typeof(tank_obj) == JSON_OBJECT, "tank element not an object");
      json_t* coords_obj = obj_get(tank_obj, "coords"); ERRCHK;
      CONDITION(json_typeof(coords_obj) == JSON_ARRAY && json_array_size(coords_obj) == 2, "tank coord should be an array of size 2");
      int coords[2];
      coords[0] = json_integer_value(expect(json_array_get(coords_obj, 0), JSON_INTEGER)); ERRCHK;
      coords[1] = json_integer_value(expect(json_array_get(coords_obj, 1), JSON_INTEGER)); ERRCHK;

      const char* type = json_string_value(expect(obj_get(tank_obj, "type"), JSON_STRING)); ERRCHK;
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
          int x = json_integer_value(expect(json_array_get(wp,0),JSON_INTEGER)); ERRCHK;
          int y = json_integer_value(expect(json_array_get(wp,1),JSON_INTEGER)); ERRCHK;
          pathWp[j] = Vector2(x,y);
        }
        path = new Path(pathSize, pathWp);
      } else {
        path = NULL;
      }
      tanks[i] = TankDescription(coords[0], coords[1], ttype, path);
    }
    /*char* jsonText = json_dumps(json,0);
    LOGE("loading level from json : %s", jsonText);
    free(jsonText);*/

    //Determine available items
    uint8_t items = 0;
    if (opt_istrue(json, "bombs"))
      items |= ITEM_BOMB;
    if (opt_istrue(json, "bounce"))
      items |= ITEM_BOUNCE;
    if (opt_istrue(json, "shield"))
      items |= ITEM_SHIELD;
    if (opt_istrue(json, "improved_firing"))
      items |= ITEM_FIRING;

    //Rewards
    eReward reward = REWARD_NONE;
    if (opt_istrue(json, "reward_bombs"))
      reward = REWARD_BOMB;
    if (opt_istrue(json, "reward_bounce"))
      reward = REWARD_BOUNCE;
    if (opt_istrue(json, "reward_shield"))
      reward = REWARD_SHIELD;
    if (opt_istrue(json, "reward_firing"))
      reward = REWARD_FIRING;

    Level* l = new Level(xdim, ydim, board, breakable, tanks, numTanks, triggers, shadows, boss, items, reward);
    delete[] board;
    delete[] breakable;
    ERRCHK;
    return l;
  }
error:
  return NULL;
}
