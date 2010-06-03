#include "TextureGroup.h"

TextureGroup::TextureGroup () : cache(NULL) {

}

TextureGroup::~TextureGroup () {
  _TextureRecord *rec;
   while(cache) {
     rec = cache;
     HASH_DEL(cache, rec); //cache automatically advanced to next
     delete rec;
   }
}

void TextureGroup::clear () {
  _TextureRecord *rec;
  while(cache) {
    rec = cache;
    HASH_DEL(cache, rec); //cache automatically advanced to next
    delete rec;
  }
}

Texture* TextureGroup::get (const char* filename) {
  _TextureRecord* r = NULL;
  HASH_FIND_STR(cache, filename, r);
  if (r == NULL) { //not found
    r = new _TextureRecord();
    r->filename = strdup(filename);
    r->tex = new Texture(filename);
    HASH_ADD_KEYPTR(hh, cache, r->filename, strlen(r->filename), r);
  }
  return r->tex;
}

