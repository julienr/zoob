#ifndef TEXTUREGROUP_H_
#define TEXTUREGROUP_H_

#include "containers/uthash.h"
#include "Texture.h"

/**
 * A TextureGroup is a set of textures that should be loaded at the same time.
 * A group can be "locked" (its textures will be loaded) or "unlocked" (its textures
 * might be unloaded if memory is needed).
 * At every state change, different groups should be locked and unlocked, depending on
 * the state requirements. For example, if we have two states "playing" and "menu", we
 * could use two TextureGroups "playing" and "menu" that would be locked only when needed.
 *
 * The locking/unlocking of TextureGroup is handled by the TextureManager
 */
class TextureGroup {
  public:
    TextureGroup ();
    ~TextureGroup ();

    //After OpenGL context is lost (after a pause), need to reload
    //all textures => this will clear the texture cache
    void clear ();

    //Returns texture id or TEXTURE_BLANK if an error
    //occured while loading the texture
    Texture* get (const char* filename);
  private:
    struct _TextureRecord {
      ~_TextureRecord () {
        free(filename);
      }
      char* filename;
      Texture* tex;
      UT_hash_handle hh;
    };

    _TextureRecord* cache;
};

#endif
