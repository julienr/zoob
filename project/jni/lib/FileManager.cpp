#include "FileManager.h"

FileManager* FileManager::instance = NULL;

/** === APK === */
APKFileManager::APKFileManager (const char* apkPath) {
  APKArchive = zip_open(apkPath, 0, NULL);
  if (APKArchive == NULL) {
    LOGE("Error loading APK : %s", apkPath);
    abort();
  }
  LOGI("Loaded APK : %s", apkPath);
}

File* APKFileManager::openFile (const char* filename) {
  zip_file* f = zip_fopen(APKArchive, filename, 0);
  if (!f) {
    LOGE("Error opening %s from APK", filename);
    return NULL;
  }
  struct zip_stat s;
  zip_stat(APKArchive, filename, 0, &s);
  return new APKFile(f, s);
}

void APKFile::read (void* buf, int nbytes) {
  zip_fread(file, buf, nbytes);
}

char* APKFile::readToBuffer (size_t* size) {
  *size = stats.size;
  char* buffer = new char[*size];
  if (!(zip_fread(file, buffer, *size) == (int)*size)) {
    LOGE("Error fully reading file to buffer");
    return NULL;
  }
  return buffer;
}

/** === Filesystem === */
FSFileManager::FSFileManager (const char* rootDir) 
 : rootDir(rootDir) {
   //TODO: check for rootDir validity ?
   LOGI("Rootdir : %s", rootDir);
}

File* FSFileManager::openFile (const char* filename) {
  char buffer[256];
  strncpy(buffer, rootDir, 256);
  strncat(buffer, filename, 256-strlen(buffer)-1);
  FILE* f = fopen(buffer, "rb");
  if (!f) {
    LOGE("Error opening %s from FS", filename);
    return NULL;
  }
  return new FSFile(f);
}

void FSFile::read (void* buf, int nbytes) {
  fread(buf, sizeof(char), nbytes, file);
}

char* FSFile::readToBuffer (size_t* size) {
  *size = getSize();
  char* buffer = new char[*size];
  if (!(fread(buffer, 1, *size, file) == *size)) {
    LOGE("Error fully reading file to buffer");
    return NULL;
  }
  return buffer;
}

size_t FSFile::getSize () {
  size_t size = 0;
  char buf[256];
  size_t numRead;
  fseek(file, 0, SEEK_SET);
  while ((numRead = fread(buf, 1, 256, file)) > 0) {
    size += numRead;
  }
  fseek(file, 0, SEEK_SET);
  return size;
}
