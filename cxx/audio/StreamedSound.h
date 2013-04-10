#ifndef STREAMED_SOUND
#define STREAMED_SOUND

#include "Sound.h"
#include "AudioStream.h"
#include "ALdllmain.h"

class StreamedSound : public Sound
{
public:

  StreamedSound(AudioStream* stream, ALuint source);
  virtual ~StreamedSound();

  void update();
  bool updateBuffer(ALuint buffer);

protected:

  ALuint mBuffers[2];
  AudioStream* mStream;
  bool mDone;
  const static int BUFFER_SIZE = 4096 * 4;
};

#endif
