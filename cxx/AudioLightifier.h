#ifndef AUDIO_LIGHTIFIER
#define AUDIO_LIGHTIFIER

#include "audio/ALSubsystem.h"
#include "fft/kiss_fft.h"
#include "fft/tools/kiss_fftr.h"

// turn PCM data into purty colors:
//
// The basic idea will be to use the frequency spectrum to modifiy the
// colors somewhat arbitrarily (this seems to be how most music visualizers
// work, it won't respond super well to beats/etc, but it should change nicely
// as the audio frequencies/etc change)
//
// Further (potential) developments:
//  - Light intensity based on sound intensity avged over a window
//  - Do simple autocorrelation to find beats?
//  - Use tone detection? (http://www.musicdsp.org/showone.php?id=107)
class AudioLightifier {
public:

  static unsigned WINDOW_SIZE;

  AudioLightifier(int num_lights);
  ~AudioLightifier();

  void setData(SoundData* d);
  void sample(uint32_t offset);

  // returns a pointer to frequency/decibel bins
  float* getBins();
  
private:
  
  SoundData* m_data;
  int m_numLights;

  // FFT output (processed into decibels)
  float* m_bins;

  kiss_fft_scalar* fft_in;
  kiss_fft_cpx* fft_out;
};

#endif
