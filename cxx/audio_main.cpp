#include <boost/python.hpp>
#include "audio/ALSubsystem.h"
#include "SDL/SDL.h"

void line(int x0, int y0, int x1, int y1, unsigned value, unsigned* buf, int stride);

ALSubsystem* audio;
SoundPtr s;
SoundData* d;
SDL_Surface* surface;
int pos = 0;
int prevx = 0, prevy = 0;

void initialize() {
  audio = new ALSubsystem();
  audio->_init();
  if (SDL_Init(SDL_INIT_VIDEO)<0)
    exit(1);
  surface = SDL_SetVideoMode(800,500,32,SDL_HWSURFACE|SDL_DOUBLEBUF);
}

// hacky bresenham
void line(int x0, int y0, int x1, int y1, unsigned value, unsigned* buf, int stride) {
  int dx = abs(x1 - x0);
  int dy = abs(y1 - y0);
  int sx = x0 < x1 ? 1 : -1;
  int sy = y0 < y1 ? 1 : -1;
  int err = dx - dy;
  while (x1 != x0 || y1 != y0) {
    if (x0 < 0 || x0 >= 800 || y0 < 0 || y0 >= 500) {
      printf("?\n");
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

void update() {
  audio->_update(0.f);
  int l = (reinterpret_cast<short*>(d->pcm))[s->getByteOffset()];
  l /= 10;
  l = std::max(-200, std::min(200, l));
  short r = (reinterpret_cast<short*>(d->pcm))[s->getByteOffset() + 2];
  printf("%010d : L %010d R %010d\n", s->getByteOffset(),
    (reinterpret_cast<short*>(d->pcm))[s->getByteOffset()],
    (reinterpret_cast<short*>(d->pcm))[s->getByteOffset() + 2]);
  unsigned* p = reinterpret_cast<unsigned*>(surface->pixels);
  line(prevx, prevy + 250, pos, l + 250, 0xffffffff, p, 800);
  *(p + (l + 250) * 800 + pos) = 0xffff00ff;

  prevx = pos;
  prevy = l;
  ++pos;
  SDL_Flip(surface);

  if (pos == 800) {
    memset(p, 0, 800*500*4);
    pos = 0;
    prevx = 0;
    prevy = 0;
  }
  usleep(10000);
}

void deinitialize() {
  delete audio;
  SDL_Quit();
}

void play_sound(const char* file) {
  s = audio->play2D(file, false);
  s->setGain(0.f, 1.f, 1.f);
  Sound* sound = &(*s);
  if (dynamic_cast<BufferedSound*>(sound)) {
    BufferedSound* bs = dynamic_cast<BufferedSound*>(sound);
    d = bs->getSoundData();
    //unsigned short* raw = reinterpret_cast<unsigned short*>(bs->getPCM());
    //for (int i = 0; i < 100; ++i) {
    //  printf("%d\n", raw[i]);
    //}
  }
}

BOOST_PYTHON_MODULE(audio) {
    using namespace boost::python;
    def("initialize", initialize);
    def("deinitialize", deinitialize);
    def("update", update);
    def("play_sound", play_sound);
}

