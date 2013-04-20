#include <boost/python.hpp>
#include "audio/ALSubsystem.h"
#include "SDL/SDL.h"
//#include <rfftw.h>
#include "AudioManager.h"

#ifndef __APPLE__
  #define DEBUG_VIZ
#endif

AudioManager* audio;

void initialize(int num_lights) {
  if (!audio) {
    audio = new AudioManager(num_lights);
  }
  audio->init();
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
}

