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
    m_lights.back().hue = 0;
    m_lights.back().bri = 128;
    m_lights.back().sat = 128;
    m_lights.back().trans = 0;
    m_lights.back().set = true;
  }
}

AudioLightifier::~AudioLightifier() {
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
  m_bassIntensity = 0.f;
  int bassSamples = 0;

  // sample decibel magnitude into bins
  for (int i = 0; i < WINDOW_SIZE / 2; ++i) {
    m_bins[i] = log10(result->r * result->r + result->i * result->i);
    if (i * (44100.f / WINDOW_SIZE) > 32.f && i * (44100.f / WINDOW_SIZE) > 512.f) {
      m_bassIntensity += m_bins[i];
      ++bassSamples;
    }
    m_binAvg += m_bins[i];
    m_binStdDev += m_bins[i] * m_bins[i];
    ++result;
  }

  m_bassIntensity /= static_cast<float>(bassSamples);
  m_binAvg /= static_cast<float>(WINDOW_SIZE / 2);
  m_binStdDev = sqrt(m_binStdDev / (WINDOW_SIZE / 2) - (m_binAvg * m_binAvg));

  computeLights();
}

float* AudioLightifier::getBins() {
  return m_bins;
}

#define CLAMP(X, MIN, MAX) std::min(MAX, std::max(MIN, X))

void AudioLightifier::computeLights() {

  //float intensity = ((m_bassIntensity - 2.f) / 7.f);
  float intensity = ((m_sampleIntensity - 2.f) / 1000.f);
  float hue = (m_binAvg - 2.f) / 4.f + 0.f;
  float sat = (m_binStdDev - 0.6f) / 3.f + 0.75;

  for (int i = 0; i < m_numLights; ++i) {
    m_lights[i].hue = static_cast<uint16_t>(static_cast<uint32_t>(hue * 65535 + i * 10000));
    m_lights[i].sat = static_cast<uint16_t>(CLAMP(sat * 254.f, 0.f, 254.f));
    m_lights[i].bri = std::max((uint16_t)10,std::min((uint16_t)150, static_cast<uint16_t>(CLAMP(intensity * 254.f, 0.f, 254.f))));
    m_lights[i].trans = 1;
  }
}

