from cxx import audio
from time import clock
from phue import *
import config

# Set up an OpenAL context and stuff
audio.initialize()

audio.play_sound("click.ogg");

while clock() < 3:
  audio.update()

# TODO implement the server part...

# Cleanup after audio stuffs
audio.deinitialize()
