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

    while True:
      # TODO process stuff from the queue!
      audio.update()

      # If we have audio playing currently, then loop through and ask
      # the audio module for each light's info
      if audio.is_audio_playing():
          for i in xrange(num_lights):
              light_info = audio.get_light(i)
              if light_info.set:
                  bridge.set_light(i + 1, {
                      'on' : True,
                      'transitiontime' : light_info.trans,
                      'hue' : light_info.hue,
                      'bri' : light_info.bri,
                      'sat' : light_info.sat
                  })
      
    # Cleanup after audio stuffs
    audio.deinitialize()

if __name__ == '__main__':
    main()
