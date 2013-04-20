#include "AudioManager.h"

#ifdef DEBUG_VIZ
static void line(int x0, int y0, int x1, int y1, unsigned value,
  unsigned* buf, int stride);
static void square(int x0, int y0, int w, int h, unsigned value,
  unsigned* buf, int stride);
#endif

AudioManager::AudioManager(int num_lights)
  :m_initialized(false),
   m_numLights(num_lights),
   m_lightifier(num_lights) {
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
    memset(reinterpret_cast<unsigned*>(m_surface->pixels) + 800*250, 0x2f, 800*250*4);
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

  m_lightifier.sample(m_nowPlaying->getByteOffset());
  //printf("%d\n", (int));

#ifdef DEBUG_VIZ


  //if (m_nowPlaying->getSecondOffset() > 45.f) {
  //  printf("!!!!\n");
  //}

  //printf("%f %d\n", m_nowPlaying->getSecondOffset(), m_nowPlaying->getByteOffset());

  if (isPlaying()) {
    //static int sp = 0;
    unsigned* p = reinterpret_cast<unsigned*>(m_surface->pixels);
    int l = (reinterpret_cast<short*>(m_data->pcm))
      [m_nowPlaying->getByteOffset()/2];
    l /= 10;
    l = std::max(-100, std::min(100, l));
    //short r = (reinterpret_cast<short*>(m_data->pcm))
      //[m_nowPlaying->getByteOffset() % AudioLightifier::WINDOW_SIZE + 2];
    //line(prevx, prevy + 350, pos, l + 350, 0xffffffff, p, 800);
    *(p + (l + 350) * 800 + pos) = 0xffff00ff;
    prevx = pos;
    prevy = l;
    ++pos;
    SDL_Flip(m_surface);
    if (pos == 800) {
      memset(p + 800*250, 0x2f, 800*250*4);
      pos = 0;
      prevx = 0;
      prevy = 0;
    }

    for (int i = 0; i < m_numLights; ++i) {
      square(50 + i * 150, 325, 100, 100, m_lightifier.getLight(i)->color, p, 800);
    }

    memset(p, 0, 800*250*4);
    float* bins = m_lightifier.getBins();

    for (int i = 0; i < AudioLightifier::WINDOW_SIZE/2 && i < 800; ++i) {
      line(i, 0, i, static_cast<int>(std::max(std::min(bins[i] * 10, 200.f), 0.f)),
        0xff0000ff, p, 800);
    }

    SDL_Flip(m_surface);
  }
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
    m_lightifier.setData(m_data);
  }
}

void AudioManager::pauseSound() { 
  if (!m_nowPlaying.isDone()) {
    m_nowPlaying->pause();
  } else if (!m_nowPlaying.isNull()) {
    m_nowPlaying.setNull();
    m_lightifier.setData(0);
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

void square(int x0, int y0, int w, int h, unsigned value, unsigned* buf, int stride) {
  for (int y = y0; y < h + y0; ++y) {
    for (int x = x0; x < w + x0; ++x) {
      *(buf + y * stride + x) = value;
    }
  }
}
#endif

