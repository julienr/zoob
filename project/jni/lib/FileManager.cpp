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
  return new APKFile(f);
}

void APKFile::read (void* buf, int nbytes) {
  zip_fread(file, buf, nbytes);
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
