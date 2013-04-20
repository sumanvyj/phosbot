from cxx import audio
from time import clock
from phue import *
import config

def main(queue=None):
    bridge = Bridge(config.BRIDGE_IP)
    lights = bridge.get_light_objects('id')
    num_lights = len(lights)

    # Set up an OpenAL context and stuff
    audio.initialize(num_lights)

    audio.play_sound("cutandrun.ogg");

    #while audio.is_audio_playing():
    while True:
      audio.update()
      #command =  {'bri' : 135, 'transitiontime' : 1, 'on' : True, 'hue' : audio.get_light(0)}
      #bridge.set_light(1, command)
      #command =  {'bri' : 135, 'transitiontime' : 1, 'on' : True, 'hue' : audio.get_light(1)}
      #bridge.set_light(2, command)
      
    # Cleanup after audio stuffs
    audio.deinitialize()

if __name__ == '__main__':
    main()
