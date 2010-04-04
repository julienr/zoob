#ifndef TEXTUREMANAGER_H_
#define TEXTUREMANAGER_H_

#include "def.h"
#include "containers/uthash.h"

#define TEXTURE_BLANK 0

class TextureManager {
public:
  static TextureManager* getInstance ();
  static void destroy();

  //Returns texture id or TEXTURE_BLANK if an error
  //occured while loading the texture
  GLuint get (const char* filename, int* width=NULL, int* height=NULL);
private:
  TextureManager();
  ~TextureManager();

  static TextureManager* instance;

  struct _TextureRecord {
    ~_TextureRecord () {
      free(filename);
    }
    GLuint glTexID;
    char* filename;
    int width, height;
    UT_hash_handle hh;
  };

  _TextureRecord* _load (const char* filename);

  _TextureRecord* cache;
};

#endif /* TEXTUREMANAGER_H_ */
