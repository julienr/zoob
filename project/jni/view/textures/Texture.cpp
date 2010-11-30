#include "Texture.h"
#include <string.h>
#include <stdlib.h>

#define TEXTURE_LOAD_ERROR 0
GLuint loadTextureFromPNG(const char* filename, int* width, int* height);

Texture::Texture (const char* name, bool loadNow) : status(UNLOADED) {
  filename = strdup(name);
  if (loadNow)
    load();
}

Texture::~Texture () {
  unload();
  free(filename);
}

void Texture::load () {
  if (status == LOADED)
    return;
  //LOGE("loading %s", filename);
  glTexID = loadTextureFromPNG(filename, &width, &height);
  if (glTexID == TEXTURE_LOAD_ERROR) {
    LOGE("Error loading texture %s", filename);
  }
  //LOGE("successful load of %s", filename);
  status = LOADED;
}

void Texture::unload () {
  if (status == UNLOADED)
    return;
  //LOGE("unloading %s", filename);
  glDeleteTextures(1, &glTexID);
  GLW::checkError("glDeleteTextures");
  status = UNLOADED;
}

#include <stdio.h>
#include "def.h"
#include <zip.h>
#include <png.h>

//Taken from http://en.wikibooks.org/wiki/OpenGL_Programming/Intermediate/Textures
/** loadTexture
 *   loads a png file into an opengl texture object, using cstdio , libpng, and opengl.
 *
 *   \param filename : the png file to be loaded
 *   \param width : width of png, to be updated as a side effect of this function
 *   \param height : height of png, to be updated as a side effect of this function
 *
 *   \return GLuint : an opengl texture id.  Will be 0 if there is a major error,
 *           should be validated by the client of this function.
 *
 */
zip_file* file;

void png_zip_read(png_structp /*png_ptr*/, png_bytep data, png_size_t length) {
  zip_fread(file, data, length);
}

GLuint loadTextureFromPNG(const char* filename, int* width, int* height) {
  file = zip_fopen(APKArchive, filename, 0);
  if (!file) {
    LOGE("Error opening %s from APK", filename);
    return TEXTURE_LOAD_ERROR;
  }

  //header for testing if it is a png
  png_byte header[8];

  //read the header
  zip_fread(file, header, 8);

  //test if png
  int is_png = !png_sig_cmp(header, 0, 8);
  if (!is_png) {
    zip_fclose(file);
    LOGE("Not a png file : %s", filename);
    return TEXTURE_LOAD_ERROR;
  }

  //create png struct
  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
      NULL, NULL);
  if (!png_ptr) {
    zip_fclose(file);
    LOGE("Unable to create png struct : %s", filename);
    return (TEXTURE_LOAD_ERROR);
  }

  //create png info struct
  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
    LOGE("Unable to create png info : %s", filename);
    zip_fclose(file);
    return (TEXTURE_LOAD_ERROR);
  }

  //create png info struct
  png_infop end_info = png_create_info_struct(png_ptr);
  if (!end_info) {
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
    LOGE("Unable to create png end info : %s", filename);
    zip_fclose(file);
    return (TEXTURE_LOAD_ERROR);
  }

  //png error stuff, not sure libpng man suggests this.
  if (setjmp(png_jmpbuf(png_ptr))) {
    zip_fclose(file);
    LOGE("Error during setjmp : %s", filename);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return (TEXTURE_LOAD_ERROR);
  }

  //init png reading
  //png_init_io(png_ptr, fp);
  png_set_read_fn(png_ptr, NULL, png_zip_read);

  //let libpng know you already read the first 8 bytes
  png_set_sig_bytes(png_ptr, 8);

  // read all the info up to the image data
  png_read_info(png_ptr, info_ptr);

  //variables to pass to get info
  int bit_depth, color_type;
  png_uint_32 twidth, theight;

  // get info about png
  png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
      NULL, NULL, NULL);

  //update width and height based on png info
  *width = twidth;
  *height = theight;

  // Update the png info struct.
  png_read_update_info(png_ptr, info_ptr);

  // Row size in bytes.
  int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  // Allocate the image_data as a big block, to be given to opengl
  png_byte *image_data = new png_byte[rowbytes * (*height)];
  if (!image_data) {
    //clean up memory and close stuff
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    LOGE("Unable to allocate image_data while loading %s ", filename);
    zip_fclose(file);
    return TEXTURE_LOAD_ERROR;
  }

  //row_pointers is for pointing to image_data for reading the png with libpng
  png_bytep *row_pointers = new png_bytep[(*height)];
  if (!row_pointers) {
    //clean up memory and close stuff
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    delete[] image_data;
    LOGE("Unable to allocate row_pointer while loading %s ", filename);
    zip_fclose(file);
    return TEXTURE_LOAD_ERROR;
  }
  // set the individual row_pointers to point at the correct offsets of image_data
  for (int i = 0; i < (*height); ++i)
    row_pointers[(*height) - 1 - i] = image_data + i * rowbytes;

  //read the png into image_data through row_pointers
  png_read_image(png_ptr, row_pointers);

  //Now generate the OpenGL texture object
  GLuint texture;
  glGenTextures(1, &texture);
  GLW::checkError("glGenTextures");
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (*width), (*height), 0, GL_RGBA,
      GL_UNSIGNED_BYTE, (GLvoid*) image_data);
  GLW::checkError("glTexImage2D");
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  //clean up memory and close stuff
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  delete[] image_data;
  delete[] row_pointers;
  zip_fclose(file);

  return texture;
}
