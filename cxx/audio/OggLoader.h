#ifndef OGG_LOADER_H
#define OGG_LOADER_H

#include "AudioLoader.h"
#include "ALdllmain.h"

struct OggVorbis_File;
typedef int64_t ogg_int64_t;

/** Loads and decodes Ogg Vorbis files */
class OggLoader : public AudioLoader
{
public: 

  OggLoader(){}
  virtual ~OggLoader(){}

  virtual void loadSound(String filename, ALuint& out, byte** buf = 0);
  virtual AudioStream* streamSound(String filename);

  static size_t ov_read_func(void *ptr, size_t size, size_t nmemb, void *datasource);
  static int ov_seek_func(void *datasource, ogg_int64_t offset, int whence);
  static int ov_close_func(void *datasource);
  static long ov_tell_func(void *datasource);

private:
    
  OggVorbis_File* loadOgg(String filename);

};

#endif
