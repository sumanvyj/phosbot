#ifndef SOUND_DATA
#define SOUND_DATA

struct SoundData {
  byte* pcm;       // ptr to buffer of raw PCM data
  size_t size;     // length of buffer in bytes
  bool stereo;     // is this a stereo sound
  bool interleaved;// are stereo channels interleaved?
  byte bits;       // generally 16 or 8
};

#endif
