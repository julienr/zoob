#include "TextureManager.h"
#include <string.h>

TextureManager* TextureManager::instance = NULL;

TextureManager::TextureManager() :
  groups(NULL) {
}

TextureManager::~TextureManager() {
  _GroupRecord *rec;
  while (groups) {
    rec = groups;
    HASH_DEL(groups, rec); //cache automatically advanced to next
    delete rec;
  }
}

void TextureManager::startLock () {
  for (_GroupRecord* r = groups; r != NULL; r = static_cast<_GroupRecord*>(r->hh.next)) {
    r->group->unlock();
  }
}

void TextureManager::lockGroup (int id) {
  getGroup(id)->lock();
}

void TextureManager::commitLock () {
  LOGE("committing lock");
  for (_GroupRecord* r = groups; r != NULL; r = static_cast<_GroupRecord*>(r->hh.next)) {
    if (r->group->isLocked()) {
      LOGE("group %i locked", r->id);
      r->group->load();
    } else
      r->group->unload();
  }
}

void TextureManager::reloadAll() {
  //This function is called when OpenGL context has been lost to reload all textures
  //Have to unload all groups so they will be reloaded correctly
  for (_GroupRecord* r = groups; r != NULL; r = static_cast<_GroupRecord*>(r->hh.next)) {
    r->group->unload();
  }
  commitLock();
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
