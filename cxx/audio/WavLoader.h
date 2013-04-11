#ifndef WAV_LOADER_H
#define WAV_LOADER_H

#include "AudioLoader.h"
#include "ALdllmain.h"

enum WavCompressionType
{
  WCT_UNKNOW = 0,
  WCT_PCM = 1,              // uncompressed (the only one this actually handles at the moment)
  WCT_MS_ADPCM = 2,         // Microsoft ADPCM
  WCT_ITU_G711_A_LAW = 6,   // ITU G.711 a-law
  WCT_ITU_G711_A_uLAW = 7,  // ITU G.711 a-ulaw
  WCT_IMA_ADPCM = 17,       // IMA ADPCM
  WCT_ITU_G723_ADPCM = 20,  // Yamaha ITU G.732 ADPCM
  WCT_GSM = 49,             // GSM 6.10
  WCT_ITU_G721_ADPCM = 64,  // IU G.721 ADPCM
  WCT_MPEG = 80,            // MPEG
  WCT_EXPERIMENTAL = 0xFFFF // Experimental
};

/** Loads plain (PCM, uncompressed) WAV files */
class WavLoader : public AudioLoader
{
public:

  WavLoader(){}
  virtual ~WavLoader(){}

  virtual void loadSound(String filename, ALuint& out, SoundData* sd = 0);
  virtual AudioStream* streamSound(String filename);

private:

  std::ifstream* loadWav(String filename, int& format, int& rate, int& dataSize);

};

#endif
