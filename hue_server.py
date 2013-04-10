from cxx import audio;
from time import clock;

# Set up an OpenAL context and stuff
audio.initialize()

#audio.play_sound("");

while clock() < 5:
  audio.update()

# TODO implement the server part...

# Cleanup after audio stuffs
audio.deinitialize()
