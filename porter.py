from cxx import audio
from time import clock
from phue import *
import config
from phue import Bridge

#b = Bridge('ip_of_your_bridge')
#b.connect()
#b.get_api()

#lights = b.get_light_objects('id')
#num_lights = len(lights)
num_lights = 2

# Set up an OpenAL context and stuff
audio.initialize(num_lights)

audio.play_sound("click.ogg");

while clock() < 3:
  audio.update()

# TODO implement the server part...

# Cleanup after audio stuffs
audio.deinitialize()
