#include "StreamedSound.h"
#include <AL/alc.h>
#include <AL/al.h>

StreamedSound::StreamedSound(AudioStream* stream, ALuint source)
  :Sound(source),mStream(stream),mDone(false)
{
  alGenBuffers(2, mBuffers);
  if(updateBuffer(mBuffers[0]))
    alSourceQueueBuffers(mSource, 1, &mBuffers[0]);
  if(updateBuffer(mBuffers[1]))
    alSourceQueueBuffers(mSource, 1, &mBuffers[1]);
}
//--------------------------------------------------------------------------

StreamedSound::~StreamedSound()
{
  mStream->close();
  stop();
  alDeleteBuffers(2, mBuffers);
}
//--------------------------------------------------------------------------

void StreamedSound::update()
{
  // once it's all been streamed, it's just a matter of waiting for the
  // buffers to be finished off...
  if(!mDone)
  {
    int processed = 0;
    alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &processed);

    // unqueue, stream to and requeue any processed buffers
    while(processed--)
    {
      ALuint buffer;
      alSourceUnqueueBuffers(mSource, 1, &buffer);

      if(updateBuffer(buffer))
        alSourceQueueBuffers(mSource, 1, &buffer);
    }
  }
}
//--------------------------------------------------------------------------

bool StreamedSound::updateBuffer(ALuint buffer)
{
  if(mDone)
    return false;

  char data[BUFFER_SIZE];
  int bytesStreamed = mStream->stream(data, BUFFER_SIZE);

  if(bytesStreamed == 0)
  {
    // no more data can be grabbed, close the source and mark it done
    mStream->close();
    mDone = true;
  }
  else
  {
    // write to the buffer
    alBufferData(buffer, mStream->getFormat(), static_cast<void*>(data), 
      bytesStreamed, mStream->getRate());
  }

  return !mDone;
}
//--------------------------------------------------------------------------

