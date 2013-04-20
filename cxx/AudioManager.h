#include <boost/python.hpp>
#include "audio/ALSubsystem.h"
#include "SDL/SDL.h"

#ifndef __APPLE__
  #define DEBUG_VIZ
#endif

class AudioManager {
public:

  AudioManager(int num_lights);
  ~AudioManager();

  void init();
  void deinit();
  void update();

  void playSound(const char* filename = 0);
  void pauseSound();
  void unpauseSound();

  void registerFile(const char* name);

  bool isPlaying();

private:

  // The audio magic
  ALSubsystem* m_audio;

  // the audio that is playing now
  SoundPtr m_nowPlaying;
  SoundData* m_data;

  // how many lights we should be spewing params for
  int m_numLights;

  bool m_initialized;

  std::vector<std::string> m_audioFiles;

#ifdef DEBUG_VIZ
  SDL_Surface* m_surface;
  int pos;
  int prevx, prevy;
#endif
};
