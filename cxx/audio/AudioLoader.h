#ifndef AUDIO_LOADER_H
#define AUDIO_LOADER_H

#include "AudioStream.h"
#include "ALdllmain.h"

typedef unsigned int ALuint;
class StreamedSound;

/** Abstract audio loading class (to be extended for each loadable filetype) */
class AudioLoader
{
public:
  
  AudioLoader(){};
  virtual ~AudioLoader(){};

  virtual void loadSound(String filename, ALuint& out, byte** buf = 0) = 0;
  virtual AudioStream* streamSound(String filename) = 0;
};

#endif

