#include <boost/python.hpp>
#include "audio/ALSubsystem.h"
#include "SDL/SDL.h"
#include <complex.h>
#include <rfftw.h>

ALSubsystem* audio;
SoundPtr s;
SoundData* d;

#ifndef __APPLE__
SDL_Surface* surface;
int pos = 0;
int prevx = 0, prevy = 0;
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
#endif

void initialize() {
  audio = new ALSubsystem();
  audio->_init();
#ifndef __APPLE__
  if (SDL_Init(SDL_INIT_VIDEO)<0)
    exit(1);
  surface = SDL_SetVideoMode(800,500,32,SDL_HWSURFACE|SDL_DOUBLEBUF);
#endif
}

void update() {
  audio->_update(0.f);
#ifndef __APPLE__
  int l = (reinterpret_cast<short*>(d->pcm))[s->getByteOffset()];
  l /= 10;
  l = std::max(-200, std::min(200, l));
  short r = (reinterpret_cast<short*>(d->pcm))[s->getByteOffset() + 2];
  //printf("%010d : L %010d R %010d\n", s->getByteOffset(),
  //  (reinterpret_cast<short*>(d->pcm))[s->getByteOffset()],
  //  (reinterpret_cast<short*>(d->pcm))[s->getByteOffset() + 2]);
  unsigned* p = reinterpret_cast<unsigned*>(surface->pixels);
  line(prevx, prevy + 250, pos, l + 250, 0xffffffff, p, 800);
  *(p + (l + 250) * 800 + pos) = 0xffff00ff;

  // dem fouriers:
  fftw_real *in, *out;
  rfftw_plan plan;

  int N = 10000;
  in = (fftw_real*) fftw_malloc(sizeof(fftw_real) * N);
  out = (fftw_real*) fftw_malloc(sizeof(fftw_real) * N);
  plan = rfftw_create_plan(N, FFTW_FORWARD, FFTW_ESTIMATE); 

  for (int i = 0; i < N; ++i) {
    *(reinterpret_cast<float*>(in) + i) =
      ((float)((reinterpret_cast<short*>(d->pcm))[s->getByteOffset() + i * 2])) / 32768.f;
  }

  rfftw_one(plan, in, out); /* repeat as needed */

  std::cout<<"---------\n";
  for (int i = 0; i < 1000; ++i) {
    if (*(reinterpret_cast<float*>(out) + i) > 10.f && *(reinterpret_cast<float*>(out) + i) < 1000.f)
      std::cout<<*(reinterpret_cast<float*>(out) + i)<<" at "<<i<<"\n";
  }

  rfftw_destroy_plan(plan);

  // ...

  fftw_free(in); fftw_free(out);

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
#endif
}

void deinitialize() {
  delete audio;
#ifndef __APPLE__
  SDL_Quit();
#endif
}

void play_sound(const char* file) {
  s = audio->play2D(file, false);
  s->setGain(0.f, 0.f, 0.f);
  Sound* sound = &(*s);
  if (dynamic_cast<BufferedSound*>(sound)) {
    BufferedSound* bs = dynamic_cast<BufferedSound*>(sound);
    d = bs->getSoundData();
  }
}

BOOST_PYTHON_MODULE(audio) {
    using namespace boost::python;
    def("initialize", initialize);
    def("deinitialize", deinitialize);
    def("update", update);
    def("play_sound", play_sound);
}

