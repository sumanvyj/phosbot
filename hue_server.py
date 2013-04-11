from cxx import audio;
from time import clock;

# Set up an OpenAL context and stuff
audio.initialize()

audio.play_sound("sample_audio/calm1.ogg");

while clock() < 30:
  audio.update()

# TODO implement the server part...

# Cleanup after audio stuffs
audio.deinitialize()
