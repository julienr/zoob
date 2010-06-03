#include "TextureManager.h"
#include <string.h>

#define TEXTURE_LOAD_ERROR 0
GLuint loadTextureFromPNG(const char* filename, int* width, int* height);

TextureManager* TextureManager::instance = NULL;

TextureManager::TextureManager() :
  groups(NULL) {
}

TextureManager::~TextureManager() {

}

void TextureManager::clear() {
  //TODO
}

void TextureManager::destroy() {
  delete instance;
  instance = NULL;
}

TextureManager* TextureManager::getInstance () {
  if (instance == NULL)
    instance = new TextureManager();
  return instance;
}

TextureGroup* TextureManager::getGroup (int id) {
  _GroupRecord* r = NULL;
  HASH_FIND_INT(groups, &id, r);
  if (r == NULL) { //not found
    r = new _GroupRecord();
    r->id = id;
    LOGE("Allocating new group with id %i", id);
    r->group = new TextureGroup();
    HASH_ADD_INT(groups, id, r);
  }
  return r->group;
}
