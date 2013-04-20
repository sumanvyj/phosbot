#include <boost/python.hpp>
#include "audio/ALSubsystem.h"
#include "SDL/SDL.h"
//#include <rfftw.h>
#include "AudioManager.h"
#include <sys/time.h>
#include <unistd.h>

#ifndef __APPLE__
  #define DEBUG_VIZ
#endif

AudioManager* audio;
struct timeval last_req;



void initialize(int num_lights) {
  if (!audio) {
    audio = new AudioManager(num_lights);
  }
  audio->init();
  gettimeofday(&last_req, 0);
}

void update() {
  audio->update();
}

void deinitialize() {
  audio->deinit();
  delete audio;
}

void play_sound(const char* file) {
  audio->playSound(file);
}

void play_sound_random() {
  audio->pauseSound();
}

void pause_sound() {
  audio->playSound();
}

void unpause_sound() {
  audio->unpauseSound();
}

void add_file(const char* name) {
  audio->registerFile(name); 
}

bool is_audio_playing() {
  audio->isPlaying();
}

Light get_light(int idx) {
  return *audio->getLightifier()->getLight(idx);
}

BOOST_PYTHON_MODULE(audio) {
    using namespace boost::python;
    def("initialize", initialize);
    def("deinitialize", deinitialize);
    def("update", update);
    def("play_sound_random", play_sound_random);
    def("pause_sound", pause_sound);
    def("unpause_sound", unpause_sound);
    def("play_sound", play_sound);
    def("add_file", add_file);
    def("is_audio_playing", is_audio_playing);
    def("get_light", get_light);
    class_<Light>("Light")
      .def_readonly("bri", &Light::bri)
      .def_readonly("hue", &Light::hue)
      .def_readonly("sat", &Light::sat)
      .def_readonly("trans", &Light::trans)
      .def_readonly("set", &Light::set);
}

