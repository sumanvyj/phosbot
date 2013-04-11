#ifndef BUFFEREDSOUND_H
#define BUFFEREDSOUND_H

#include "ALdllmain.h"
#include "Sound.h"

class BufferedSound : public Sound
{
public:

  BufferedSound(ALuint source, ALuint buffer, const SoundData& data);
  virtual ~BufferedSound();
  virtual void update();
  
  SoundData* getSoundData();

protected:

  ALuint mBuffer;
  SoundData mData;

};

#endif
