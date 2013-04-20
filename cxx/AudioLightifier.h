#ifndef AUDIO_LIGHTIFIER
#define AUDIO_LIGHTIFIER

#include "audio/ALSubsystem.h"
#include "fft/kiss_fft.h"
#include "fft/tools/kiss_fftr.h"

struct Light {
  uint16_t hue;
  uint8_t bri;
  uint8_t sat;
  uint16_t trans;
  bool set;
};

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

  class fColor {
  public:
    float r,g,b;
    fColor(float r_ = 0.f, float g_ = 0.f, float b_ = 0.f) 
      :r(r_),g(g_),b(b_) {}

    uint32_t toRGB() {
      unsigned char rc = std::min(255.0f, std::max(0.0f, r * 255.0f));
      unsigned char gc = std::min(255.0f, std::max(0.0f, g * 255.0f));
      unsigned char bc = std::min(255.0f, std::max(0.0f, b * 255.0f));
      return (rc << 16) | (gc << 8) | (bc);
    }
  };

  AudioLightifier(int num_lights);
  ~AudioLightifier();

  void setData(SoundData* d);
  void sample(uint32_t offset);

  // returns a pointer to frequency/decibel bins
  float* getBins();

  Light* getLight(int idx) { return &m_lights[idx]; }
  
private:

  void computeLights();

  SoundData* m_data;
  int m_numLights;

  // FFT output (processed into decibels)
  float* m_bins;

  float m_binAvg;
  float m_binStdDev;
  float m_sampleIntensity;

  kiss_fft_scalar* fft_in;
  kiss_fft_cpx* fft_out;

  fColor prevColor;

  std::vector<Light> m_lights;
};

#endif
