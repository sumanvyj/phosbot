#include "ALdllmain.h"
#include "WavLoader.h"
#include "WavStream.h"

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

void getChunkId(std::ifstream* stream, char* out)
{
  stream->read(out, 4);
}
//---------------------------------------------------------------------------

uint32_t getChunkUInt32(std::ifstream* stream)
{
  char bytes[4];
  stream->read(bytes, 4);
  return *reinterpret_cast<uint32_t*>(bytes);
}
//---------------------------------------------------------------------------

uint16_t getChunkUInt16(std::ifstream* stream)
{
  char bytes[2];
  stream->read(bytes, 2);
  return *reinterpret_cast<uint16_t*>(bytes);
}
//---------------------------------------------------------------------------

void WavLoader::loadSound(String filename, ALuint& out, SoundData* sd)
{
  int sampleRate;
  int format;
  int dataSize;

  std::ifstream* file = loadWav(filename, format, sampleRate, dataSize);

  char* data = 0;
  // read in the entire contents of the data chunk
  if (!sd) {
    data = new char[dataSize];
  } else {
    sd->pcm = new byte[dataSize];
    sd->size = dataSize;
    sd->stereo = format == AL_FORMAT_STEREO16 || format == AL_FORMAT_STEREO8;
    sd->bits = (AL_FORMAT_MONO8 || format == AL_FORMAT_STEREO8) ? 8 : 16;
    sd->interleaved = true; // at least i sure hope so...
    data = reinterpret_cast<char*>(sd->pcm);
  }
  //char* data = new char[dataSize];
  file->read(data, dataSize);

  // create and write to output buffer
  alGenBuffers(1, &out);
  alBufferData(out, format, static_cast<void*>(data), dataSize, sampleRate);

  // cleanup
  delete file;
  if (!sd) delete[] data;
  file->close();
}
//---------------------------------------------------------------------------

AudioStream* WavLoader::streamSound(String filename)
{
  int sampleRate;
  int format;
  int dataSize;
  std::ifstream* file = loadWav(filename, format, sampleRate, dataSize);
  return new WavStream(file, format, sampleRate, dataSize);
}
//---------------------------------------------------------------------------

std::ifstream* WavLoader::loadWav(String filename, int& format, int& rate, int& outSize)
{
  std::ifstream* file = new std::ifstream(filename.c_str(), std::ios::in | std::ios::binary);

  if(!file->is_open())
    throw Exception("Could not open file.");

  // process the header
  char headerId[4];
  char riffType[4];
  uint32_t size;

  getChunkId(file, headerId);
  size = getChunkUInt32(file);
  getChunkId(file, riffType);

  // invalid wav
  if(String(headerId, 4) != "RIFF" || String(riffType, 4) != "WAVE")
    throw Exception("Invalid WAV file.");

  // keep track of how many bytes have been processed
  uint32_t processed = 8;

  // data
  byte* data = 0;
  uint32_t dataSize = 0;

  // format info
  WavCompressionType compression;
  uint16_t numChannels;
  uint32_t sampleRate;
  uint32_t bytesPerSec;
  uint16_t blockAlign;
  uint16_t significantBitsPerSample;

  // process chunks
  while(file->good() && processed < size)
  {
    char chunkId[4];
    uint32_t chunkSize;
  
    // assume we're starting on a chunk, this could go very wrong if we don't
    // do the chunk offsets correctly...
    getChunkId(file, chunkId);
    processed += 4;
    chunkSize = getChunkUInt32(file);
    processed += 4;

    if(String(chunkId,3) == "fmt")
    {
      compression = static_cast<WavCompressionType>(getChunkUInt16(file));
      processed += 2;

      numChannels = getChunkUInt16(file);
      processed += 2;
      
      sampleRate = getChunkUInt32(file);
      processed += 4;

      bytesPerSec = getChunkUInt32(file);
      processed += 4;

      blockAlign = getChunkUInt16(file);
      processed += 2;

      significantBitsPerSample = getChunkUInt16(file);
      processed += 2;

      // skip any extra formating bytes
      file->ignore(chunkSize - 16);
    }
    else if(String(chunkId,4) == "data")
    {
      data = new byte[chunkSize];
      dataSize = chunkSize;
      processed += chunkSize;
      // break right at the start of the data...
      break;
    }
    else // some other chunk type, ignore for now
    {
      file->ignore(chunkSize);
      processed += chunkSize;
    }

    // skip padding byte if chunk data isn't 2 byte aligned
    if(chunkSize & 1)
    {
      file->ignore(1);
      processed += 1;
    }
  }

  // determine format (this won't handle stuff with >2 channels..)
  format = numChannels == 1 ? 
    blockAlign == 2 ? AL_FORMAT_MONO16   : AL_FORMAT_MONO8:
    blockAlign == 4 ? AL_FORMAT_STEREO16 : AL_FORMAT_STEREO8;
  rate = sampleRate;
  outSize = dataSize;

  return file;
}
//---------------------------------------------------------------------------
