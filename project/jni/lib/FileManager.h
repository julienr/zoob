#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

#include "def.h"

class FileManager;

class File {
  friend class FileManager;
  public:
    virtual ~File () {}

    virtual void read (void* buf, int nbytes) = 0;

    /**
     * Fully read the file to a newly allocated buffer of corresponding
     * size. The returned buffer MUST be freed by the caller using delete []
     * @param size OUT parameter, will contain the size of the buffer
     */
    virtual char* readToBuffer (size_t* size) = 0;
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

    char* readToBuffer (size_t* size);

  private:
    APKFile(zip_file* file, struct zip_stat stats)
      : file(file), stats(stats) {} 
    zip_file* file;
    struct zip_stat stats;
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

    char* readToBuffer(size_t* size);

  private:
    FSFile(FILE* file)
      : file(file) {} 

    size_t getSize ();

    FILE* file;
};

#endif
