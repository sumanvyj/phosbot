#include "OggLoader.h"
#include "OggStream.h"

#include <AL/alc.h>
#include <AL/al.h>

#include <vorbis/vorbisfile.h>

void OggLoader::loadSound(String filename, ALuint& out, byte** buf)
{
  // do basic opening stuff
  OggVorbis_File* oggFile = loadOgg(filename);
  
  // grab info
  vorbis_info* info = ov_info(oggFile, -1);

  // determine format based on channels (always 16 bit)
  int format = info->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

  // determine uncompressed PCM size (assuming stream formats within the file are consistent..)
  int size = ov_pcm_total(oggFile, -1) * info->channels * 2;

  char* data;

  // allocate buffer
  if (!buf) {
    data = new char[size];
  } else {
    *buf = new byte[size];
    data = reinterpret_cast<char*>(*buf);
  }

  int pos = 0;
  int section; // not used atm...

  // read it all into the buffer
  while(pos < size)
  {
    int result = ov_read(oggFile, data + pos, size - pos, 0, 2, 1, &section);

    if(result > 0)
      pos += result;
    else if(result == 0)
      break;
    else
      throw Exception("Vorbis Error! "/* + StringUtils::toString(result)*/);
  }

  // create and write to output buffer
  alGenBuffers(1, &out);
  alBufferData(out, format, static_cast<void*>(data), size, info->rate);
  
  // cleanup
  if (!buf) {
    delete[] data;
  }
  ov_clear(oggFile);
  free(oggFile);
}
//---------------------------------------------------------------------------

AudioStream* OggLoader::streamSound(String filename)
{
  // open and init
  OggVorbis_File* file = loadOgg(filename);

  // grab info struct
  vorbis_info* info = ov_info(file, -1);

  // sort out format stuff
  int format = info->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
  int sampleRate = info->rate;

  // return stream object
  return new OggStream(file, format, sampleRate);
}
//---------------------------------------------------------------------------

size_t OggLoader::ov_read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{
  return fread(ptr, size, nmemb, (FILE*)datasource);
}
//---------------------------------------------------------------------------

int OggLoader::ov_seek_func(void *datasource, ogg_int64_t offset, int whence)
{
  return fseek((FILE*)datasource, (long)offset, whence);
}
//---------------------------------------------------------------------------

int OggLoader::ov_close_func(void *datasource)
{
   return fclose((FILE*)datasource);
}
//---------------------------------------------------------------------------

long OggLoader::ov_tell_func(void *datasource)
{
  return ftell((FILE*)datasource);
}
//---------------------------------------------------------------------------

OggVorbis_File* OggLoader::loadOgg(String filename)
{
  FILE* file;
  OggVorbis_File* oggFile = static_cast<OggVorbis_File*>(malloc(sizeof(OggVorbis_File)));

  // open the file up
  file = fopen(filename.c_str(), "rb");

  if(!file)
    throw Exception("Could not open Ogg file.");

  ov_callbacks callbacks;
  callbacks.read_func = ov_read_func;
  callbacks.seek_func = ov_seek_func;
  callbacks.close_func = ov_close_func;
  callbacks.tell_func = ov_tell_func;

  // open the stream, with custom callbacks
  if(ov_open_callbacks(file, oggFile, 0, 0, callbacks) < 0)
  {
    fclose(file);
    throw Exception("Could not open Ogg stream.");
  }

  return oggFile;
}
//---------------------------------------------------------------------------

