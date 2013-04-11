#include "ALdllmain.h"
#include "BufferedSound.h"

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

BufferedSound::BufferedSound(ALuint source, ALuint buffer, const SoundData& data)
  :Sound(source),mBuffer(buffer),mData(data)
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

SoundData* BufferedSound::getSoundData() {
  return &mData;
}

