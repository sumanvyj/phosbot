#ifndef WAV_STREAM
#define WAV_STREAM

#include "AudioStream.h"
#include "ALdllmain.h"

class ORYX_AL_EXPORT WavStream : public AudioStream
{
public:

  WavStream(std::ifstream* file, int format, int rate, int dataSize);
  virtual ~WavStream();

  virtual int stream(char* buffer, int length);
  virtual void close();

private:

  std::ifstream* mFile;
  int mSize;
  int mPosition;

};

#endif
