add_library(audio SHARED audio_main.cpp)
set_target_properties(audio PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(audio ${Boost_LIBRARIES})
