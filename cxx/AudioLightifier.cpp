#include "AudioLightifier.h"

unsigned AudioLightifier::WINDOW_SIZE = 2048;

AudioLightifier::AudioLightifier(int num_lights) {
  m_data = 0;
  m_numLights = num_lights;
  m_bins = new float[WINDOW_SIZE/2];
  fft_in = new kiss_fft_scalar[WINDOW_SIZE];
  fft_out = new kiss_fft_cpx[WINDOW_SIZE / 2 + 1];
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

  short* pcm = reinterpret_cast<short*>(m_data->pcm);

  // sample starting at offset
  for (int i = 0; i < WINDOW_SIZE; ++i) {
    fft_in[i] = pcm[(offset + i) * 2];
  }

  // run real fft (note that we get complex results)
  kiss_fftr_cfg fft_conf = kiss_fftr_alloc(WINDOW_SIZE, 0, 0, 0);
  kiss_fftr(fft_conf, fft_in, fft_out);

  kiss_fft_cpx* result = fft_out;
  ++result; // skip DC

  // sample decibel magnitude into bins
  for (int i = 0; i < WINDOW_SIZE / 2; ++i) {
    m_bins[i] = log10(result->r * result->r + result->i * result->i);
    ++result;
  }
}

float* AudioLightifier::getBins() {
  return m_bins;
}

