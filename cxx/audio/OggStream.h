#ifndef OGG_STREAM
#define OGG_STREAM

#include "AudioStream.h"

struct OggVorbis_File;

class ORYX_AL_EXPORT OggStream : public AudioStream
{
public:

  OggStream(OggVorbis_File* file, int format, int rate);
  virtual ~OggStream();

  virtual int stream(char* buffer, int length);
  virtual void close();

private:

  OggVorbis_File* mFile;

};

#endif

