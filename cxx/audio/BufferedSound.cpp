#include "ALdllmain.h"
#include "BufferedSound.h"

#include <AL/al.h>
#include <AL/alc.h>

BufferedSound::BufferedSound(ALuint source, ALuint buffer, byte* pcm)
  :Sound(source),mBuffer(buffer),mPCM(pcm)
{
  // set buffer up, don't play automatically though
  alSourcei(mSource, AL_BUFFER, mBuffer);
}

BufferedSound::~BufferedSound()
{
  // ... nothin' to see here ...
}

void BufferedSound::update()
{
  // ... nothin' to see here ...
}

byte* BufferedSound::getPCM() {
  return mPCM;
}

