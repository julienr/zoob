#ifndef TEXTUREMANAGER_H_
#define TEXTUREMANAGER_H_

#include "def.h"
#include <GLES/gl.h>
#include "containers/uthash.h"

class TextureManager {
public:
  static TextureManager* getInstance ();

  GLuint get (const char* filename);
private:
  TextureManager();

  static TextureManager* instance;

  GLuint _load (const char* filename);

  struct _TextureRecord {
    GLuint glTexID;
    const char* filename;
    int width, height;
    UT_hash_handle hh;
  };

  _TextureRecord* cache;
};

#endif /* TEXTUREMANAGER_H_ */
