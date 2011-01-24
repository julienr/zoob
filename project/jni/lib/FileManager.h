#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

#include "def.h"

class FileManager;

class File {
  friend class FileManager;
  public:
    virtual ~File () {}

    virtual void read (void* buf, int nbytes) = 0;
  private:
};

class FileManager {
  /** Singleton */
  private:
    static FileManager* instance;

  protected:
    FileManager () {}

  public:
    static void registerInstance (FileManager* inst) {
      instance = inst;
    }

    static FileManager* getInstance () {
      return instance;
    }

    static void destroy () {
      delete instance;
      instance = NULL;
    }

    virtual File* openFile (const char* filename) = 0;
};

/** === APK === */
class APKFileManager : public FileManager {
  public:
    APKFileManager (const char* apkPath);

    File* openFile (const char* filename);
  private:
    zip* APKArchive;
};

class APKFile : public File {
  friend class APKFileManager;
  public:
    ~APKFile() {
      zip_fclose(file);
    }

    void read (void* buf, int nbytes);

  private:
    APKFile(zip_file* file)
      : file(file) {} 
    zip_file* file;
};

/** === Filesystem === */

class FSFileManager : public FileManager {
  public:
    FSFileManager (const char* rootDir);

    File* openFile (const char* filename);
  private:
    const char* rootDir;
};

class FSFile : public File {
  friend class FSFileManager;
  public:
    ~FSFile() {
      fclose(file);
    }

    void read (void* buf, int nbytes);

  private:
    FSFile(FILE* file)
      : file(file) {} 
    FILE* file;
};

#endif
