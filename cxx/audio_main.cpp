#include <boost/python.hpp>
#include "audio/ALSubsystem.h"

ALSubsystem* audio;

void initialize() {
  std::cout<<"Initializing audio system...\n";
  audio = new ALSubsystem();
  audio->_init();
  std::cout<<"Audio initialized.\n";
}

void update() {
  audio->_update(0.f);
}

void deinitialize() {
  std::cout<<"Deinitializing audio system...\n";
  delete audio;
  std::cout<<"Audio deinitialized.\n";
}

void play_sound(const char* file) {
  SoundPtr s = audio->play2D(file, false);
  s->setGain(0.f, 1.f, 1.f);
  Sound* sound = &(*s);
  if (dynamic_cast<BufferedSound*>(sound)) {
    BufferedSound* bs = dynamic_cast<BufferedSound*>(sound);
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

