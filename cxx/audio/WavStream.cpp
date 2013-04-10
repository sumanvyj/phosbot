#include "WavStream.h"

WavStream::WavStream(std::ifstream* file, int format, int rate, int dataSize)
  :AudioStream(format, rate), mFile(file), mSize(dataSize), mPosition(0)
{}
//---------------------------------------------------------------------------

WavStream::~WavStream()
{
  close();
}
//---------------------------------------------------------------------------

int WavStream::stream(char* buffer, int length)
{
  if(mSize == mPosition)
  {
    return 0;
  }
  else if(mSize - mPosition <= length)
  {
    mFile->read(buffer, mSize - mPosition);
    int wrote = mSize - mPosition;
    mPosition = mSize;
    return wrote;
  }
  else
  {
    mFile->read(buffer, length);
    mPosition += length;
    return length;
  }
}
//---------------------------------------------------------------------------

void WavStream::close()
{	
  delete mFile;
  mFile = 0;
}
//---------------------------------------------------------------------------

