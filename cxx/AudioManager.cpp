#include "AudioManager.h"

static void line(int x0, int y0, int x1, int y1, unsigned value,
  unsigned* buf, int stride);

AudioManager::AudioManager(int num_lights)
  :m_initialized(false),
   m_numLights(num_lights) {
#ifdef DEBUG_VIZ
  pos = 0;
  prevx = 0;
  prevy = 0;
#endif
}

AudioManager::~AudioManager() {
  if (m_initialized) {
    deinit();
  }
}

void AudioManager::init() {
  if (!m_initialized) {
    m_audio = new ALSubsystem();
    m_audio->_init();
    m_initialized = true;
#ifdef DEBUG_VIZ
    if (SDL_Init(SDL_INIT_VIDEO)<0)
      exit(1);
    m_surface = SDL_SetVideoMode(800,500,32,SDL_HWSURFACE|SDL_DOUBLEBUF);
#endif
  }
}

void AudioManager::deinit() {
  if (m_initialized) {
    m_audio->_deinit();
    delete m_audio;
    m_audio = NULL;
    m_initialized = false;
#ifdef DEBUG_VIZ
    SDL_Quit();
#endif
  }
}

void AudioManager::update() {
  m_audio->_update(0.f);

#ifdef DEBUG_VIZ
  int l = (reinterpret_cast<short*>(m_data->pcm))[m_nowPlaying->getByteOffset()];
  l /= 10;
  l = std::max(-200, std::min(200, l));
  short r = (reinterpret_cast<short*>(m_data->pcm))
    [m_nowPlaying->getByteOffset() + 2];
  unsigned* p = reinterpret_cast<unsigned*>(m_surface->pixels);
  line(prevx, prevy + 250, pos, l + 250, 0xffffffff, p, 800);
  *(p + (l + 250) * 800 + pos) = 0xffff00ff;
  prevx = pos;
  prevy = l;
  ++pos;
  SDL_Flip(m_surface);
  if (pos == 800) {
    memset(p, 0, 800*500*4);
    pos = 0;
    prevx = 0;
    prevy = 0;
  }
  usleep(10000);
#endif
}

void AudioManager::playSound(const char* filename) {
  if (!filename) {
    if (m_audioFiles.empty()) {
      printf("NOOOOOO... audio files\n");
      exit(1);
    }
    // select a random song
    filename = m_audioFiles[rand() % m_audioFiles.size()].c_str();
  }
  m_nowPlaying = m_audio->play2D(filename, false);
  m_nowPlaying->setGain(0.f, 1.f, 1.f);
  Sound* sound = &(*m_nowPlaying);
  if (dynamic_cast<BufferedSound*>(sound)) {
    BufferedSound* bs = dynamic_cast<BufferedSound*>(sound);
    m_data = bs->getSoundData();
  }
}

void AudioManager::pauseSound() { 
  if (!m_nowPlaying.isDone()) {
    m_nowPlaying->pause();
  } else if (!m_nowPlaying.isNull()) {
    m_nowPlaying.setNull();
  }
}

void AudioManager::unpauseSound() { 
  if (!m_nowPlaying.isDone() && !m_nowPlaying.isNull()
    && m_nowPlaying->isPaused()) {
    m_nowPlaying->play();
  }
}

void AudioManager::registerFile(const char* name) {
  m_audioFiles.push_back(std::string(name));
}

bool AudioManager::isPlaying() {
  return !m_nowPlaying.isNull() && m_nowPlaying->isPlaying();
}

#ifdef DEBUG_VIZ
// hacky bresenham
void line(int x0, int y0, int x1, int y1, unsigned value, unsigned* buf, int stride) {
  int dx = abs(x1 - x0);
  int dy = abs(y1 - y0);
  int sx = x0 < x1 ? 1 : -1;
  int sy = y0 < y1 ? 1 : -1;
  int err = dx - dy;
  while (x1 != x0 || y1 != y0) {
    if (x0 < 0 || x0 >= 800 || y0 < 0 || y0 >= 500) {
      // ruh roh
    } else {
      *(buf + y0 * stride + x0) = value;
    }
    int e2 = err*2;
    if (e2 > -dy) {
      err -= dy;
      x0 += sx;
    }
    if (e2 < dx) {
      err += dx;
      y0 += sy;
    }
  }
}
#endif

