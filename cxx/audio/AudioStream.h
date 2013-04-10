#ifndef AUDIO_STREAM_H
#define AUDIO_STREAM_H

#include "ALdllmain.h"

typedef unsigned int ALuint;

/** An abstract streaming audio source */
class ORYX_AL_EXPORT AudioStream
{
public:

  AudioStream(int format, int rate)
    :mRate(rate),mFormat(format){}

  virtual ~AudioStream(){}

  /** Stream into the provided buffer */ 
  virtual int stream(char* buffer, int length) = 0;

  /** Close and free up the source */
  virtual void close() = 0;

  int getRate(){return mRate;}
  int getFormat(){return mFormat;}

protected:

  int mRate;
  int mFormat;

};

#endif

