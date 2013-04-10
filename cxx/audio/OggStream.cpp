#include "OggStream.h"
#include <vorbis/vorbisfile.h>

OggStream::OggStream(OggVorbis_File* file, int format, int rate)
  :AudioStream(format, rate), mFile(file)
{}
//---------------------------------------------------------------------------

OggStream::~OggStream()
{
  close();
}
//---------------------------------------------------------------------------

int OggStream::stream(char* buffer, int length)
{
  int bytesWritten = 0;
  int section;

  while(bytesWritten < length)
  {
    int wrote = ov_read(mFile, buffer + bytesWritten, 
      length - bytesWritten, 0, 2, 1, &section);
    if(wrote > 0)
      bytesWritten += wrote;
    else if(wrote < 0)
      return 0;
    else
      break;
  }

  return bytesWritten;
}
//---------------------------------------------------------------------------

void OggStream::close()
{	
  if(mFile)
    ov_clear(mFile);
  free(mFile);
  mFile = 0;
}
//---------------------------------------------------------------------------

