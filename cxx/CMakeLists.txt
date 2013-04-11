add_library(audio SHARED
  audio_main.cpp
  audio/ALdllmain.cpp
  audio/ALSubsystem.cpp
  audio/BufferedSound.cpp
  audio/OggLoader.cpp
  audio/OggStream.cpp
  audio/Sound.cpp
  audio/StreamedSound.cpp
  audio/WavLoader.cpp
  audio/WavStream.cpp
  audio/Vector3.cpp
)
include_directories("/usr/local/include")
set_target_properties(audio PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(audio ${Boost_LIBRARIES} openal vorbis vorbisfile ogg SDL)
