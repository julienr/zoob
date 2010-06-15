#include "GLW.h"
#define GL_GLEXT_PROTOTYPES
#include <glext.h>

//http://www.opengl.org/resources/features/OGLextensions/
bool GLW::isExtensionSupported(const char *extension) {
  char *extensions = (char *)glGetString(GL_EXTENSIONS);
  return strstr(extensions, extension);
}
