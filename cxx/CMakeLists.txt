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
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/local/lib)
include_directories("/usr/local/include")
set_target_properties(audio PROPERTIES PREFIX "")

if (APPLE)
  set (OPENAL_LIB "/System/Library/Frameworks/OpenAL.framework/OpenAL")
else ()
  set (OPENAL_LIB openal)
endif ()

TARGET_LINK_LIBRARIES(audio ${Boost_LIBRARIES} ${OPENAL_LIB} vorbis vorbisfile ogg SDL)
