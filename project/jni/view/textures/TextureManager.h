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

  /** On android, when the application goes to sleep mode, the OpenGL context is lost and all
   * resources (such as textures) must be reloaded. This function will reload all the locked TextureGroup
   */
  void reloadAll ();

  TextureGroup* getGroup (int id);

  //Group locking. A locked texture group will retain all its texture in video memory, while
  //an unlocked group might free some of them

  //Before changing locking, startLock should be called, then lock any number of groups and then call commitLock to apply
  void startLock();
  void commitLock (); //load all locked groups, unload all unlocked
  void lockGroup (int id);
private:
  TextureManager();
  ~TextureManager();
  static TextureManager* instance;

  struct _GroupRecord {
      ~_GroupRecord () {
        delete group;
      }
      int id;
      TextureGroup* group;
      UT_hash_handle hh;
  };

  _GroupRecord* groups;
};

#endif /* TEXTUREMANAGER_H_ */
