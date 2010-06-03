#ifndef TEXTUREMANAGER_H_
#define TEXTUREMANAGER_H_

#include "def.h"
#include "containers/uthash.h"
#include "view/GLW.h"
#include "TextureGroup.h"

#define TEXTURE_BLANK 0

class TextureManager {
public:
  static TextureManager* getInstance ();
  static void destroy();

  TextureGroup* getGroup (int id);

  //After OpenGL context is lost (after a pause), need to reload
  //all textures => this will clear the texture cache
  void clear ();
private:
  TextureManager();
  ~TextureManager();
  static TextureManager* instance;

  struct _GroupRecord {
      int id;
      TextureGroup* group;
      UT_hash_handle hh;
  };

  _GroupRecord* groups;
};

#endif /* TEXTUREMANAGER_H_ */
