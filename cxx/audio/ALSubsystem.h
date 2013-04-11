#ifndef OAL_SUBSYSTEM_H
#define OAL_SUBSYSTEM_H

#include "ALdllmain.h"
#include "Sound.h"
#include "AudioLoader.h"
#include "WavLoader.h"
#include "OggLoader.h"
#include "BufferedSound.h"
#include "StreamedSound.h"

typedef unsigned int ALuint;
struct ALCdevice_struct;
typedef struct ALCdevice_struct ALCdevice;
struct ALCcontext_struct;
typedef struct ALCcontext_struct ALCcontext;

class ALSubsystem
{
public:

  ALSubsystem();
  ~ALSubsystem();

  virtual void _init();
  virtual void _deinit();
  virtual void _update(Real delta);
  virtual void _endState();
  virtual String getName();

  SoundPtr play2D(String name, bool startPaused = false);
  SoundPtr stream2D(String filename, bool startPaused = false);

  void loadSound(String filename, String name = "AUTO");
  void unloadSound(String name);

  void registerAudioLoader(AudioLoader* loader, String filetype);

protected:

  ALuint getSource();
  bool hasSources();

  bool mInitialized;

  ALCdevice* mDevice;
  ALCcontext* mContext;

  std::list<SoundPtr> mActiveSounds;
  std::vector<ALuint> mSources;
  std::map<ALuint, bool> mSourceAvailability;
  std::map<String, ALuint> mAudioBuffers;
  std::map<String, SoundData> mAudioData;
  std::map<String, AudioLoader*> mAudioLoaders; 

  // Built-in loaders
  WavLoader mWavLoader;
  OggLoader mOggLoader;
};

#endif

