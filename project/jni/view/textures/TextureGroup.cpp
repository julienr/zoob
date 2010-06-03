#include "TextureGroup.h"

TextureGroup::TextureGroup () : locked(false), cache(NULL) {

}

TextureGroup::~TextureGroup () {
  _TextureRecord *rec;
   while(cache) {
     rec = cache;
     HASH_DEL(cache, rec); //cache automatically advanced to next
     delete rec;
   }
}

void TextureGroup::load () {
  for (_TextureRecord* r = cache; r != NULL; r = static_cast<_TextureRecord*>(r->hh.next)) {
    r->tex->load();
  }
}

void TextureGroup::unload () {
  for (_TextureRecord* r = cache; r != NULL; r = static_cast<_TextureRecord*>(r->hh.next)) {
    r->tex->unload();
  }
}

Texture* TextureGroup::get (const char* filename) {
  _TextureRecord* r = NULL;
  HASH_FIND_STR(cache, filename, r);
  if (r == NULL) { //not found
    r = new _TextureRecord();
    r->filename = strdup(filename);
    r->tex = new Texture(filename, locked);
    HASH_ADD_KEYPTR(hh, cache, r->filename, strlen(r->filename), r);
  }
  return r->tex;
}

