#include "AudioLightifier.h"

unsigned AudioLightifier::WINDOW_SIZE = 2048;

AudioLightifier::AudioLightifier(int num_lights) {
  m_data = 0;
  m_numLights = num_lights;
  m_bins = new float[WINDOW_SIZE/2];
  fft_in = new kiss_fft_scalar[WINDOW_SIZE];
  fft_out = new kiss_fft_cpx[WINDOW_SIZE / 2 + 1];
  for (int i = 0; i < num_lights; ++i) {
    m_lights.push_back(Light());
    m_lights.back().color = 0x0;
  }
}

AudioLightifier::~AudioLightifier() {
  // do stuff here...
  delete[] fft_out;
  delete[] fft_in;
  delete[] m_bins;
}

void AudioLightifier::setData(SoundData* d) {
  m_data = d;
}

void AudioLightifier::sample(uint32_t offset) {
  // starting from some offset, run an FFT on WINDOW_SIZE
  // samples (at 1024 on 44.1kHz, this'll be ~20ms of audio)

  offset += (0.01f * 44100) * 2;

  short* pcm = reinterpret_cast<short*>(m_data->pcm);
  m_sampleIntensity = 0.f;

  // sample starting at offset
  for (int i = 0; i < WINDOW_SIZE; ++i) {
    fft_in[i] = pcm[offset / 2 + (m_data->stereo ? i * 2 : i)];
    m_sampleIntensity += fabs(fft_in[i]);
  }

  m_sampleIntensity /= WINDOW_SIZE;

  // run real fft (note that we get complex results)
  kiss_fftr_cfg fft_conf = kiss_fftr_alloc(WINDOW_SIZE, 0, 0, 0);
  kiss_fftr(fft_conf, fft_in, fft_out);

  kiss_fft_cpx* result = fft_out;
  ++result; // skip DC

  m_binAvg = 0.f;
  m_binStdDev = 0.f;

  // sample decibel magnitude into bins
  for (int i = 0; i < WINDOW_SIZE / 2; ++i) {
    m_bins[i] = log10(result->r * result->r + result->i * result->i);
    m_binAvg += m_bins[i];
    m_binStdDev += m_bins[i] * m_bins[i];
    ++result;
  }

  m_binAvg /= static_cast<float>(WINDOW_SIZE / 2);
  m_binStdDev = sqrt(m_binStdDev / (WINDOW_SIZE / 2) - (m_binAvg * m_binAvg));

  computeLights();
}

float* AudioLightifier::getBins() {
  return m_bins;
}

void AudioLightifier::computeLights() {
  // TODO come up with random numbers out of FFT bins to plug into color

  fColor c(0.f, 0.f,0.f);
  //c.r = m_binStdDev / 2.f + 0.2f;
  //c.g = log(m_binAvg) + 0.3f;
  c.r = log(m_sampleIntensity) / 20.f;// + 0.2f;
  c.g = log(m_sampleIntensity) / 20.f;// + 0.2f;
  c.b = log(m_sampleIntensity) / 20.f;// + 0.2f;
  //fColor tmp = c;
  for (int i = 0; i < m_numLights; ++i)  {
    m_lights[i].color = c.toRGB();
  }
  float intensity = log(m_sampleIntensity) / 20.f + 0.5f;
  //c.b = 0.f;
  //printf("%f\n", m_binStdDev);
  c.g = (m_binAvg - 5.f) / 45.f + 0.6f;
  c.b = (m_binStdDev - 0.6f) / 2.f + 0.85f;
  c.g *= intensity;
  c.b *= intensity;
  m_lights[0].color = c.toRGB();
  c.b = (m_binAvg - 5.f) / 30.f + 0.6f;
  c.g = (m_binStdDev - 0.6f) / 1.8f + 0.75f;
  c.g *= intensity;
  c.b *= intensity;
  m_lights[1].color = c.toRGB();
  /*c.r += prevColor.r;
  fColor tmp = c;
  c.g += prevColor.g;
  c.b += prevColor.b;
  c.r /= 2.f;
  c.g /= 2.f;
  c.b /= 2.f;*/
  //prevColor = tmp;
}

