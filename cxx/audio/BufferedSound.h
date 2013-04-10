#ifndef BUFFEREDSOUND_H
#define BUFFEREDSOUND_H

#include "ALdllmain.h"
#include "Sound.h"

class BufferedSound : public Sound
{
public:

  BufferedSound(ALuint source, ALuint buffer, byte* pcm);
  virtual ~BufferedSound();
  virtual void update();
  
  byte* getPCM();

protected:

  ALuint mBuffer;
  byte* mPCM;

};

#endif
