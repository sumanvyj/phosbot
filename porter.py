from cxx import audio
from time import clock
from phue import *
import config
import time
import os
import sys

SONGS_PATH = 'SONGS_PATH'

def main(queue=None):
    bridge = Bridge(config.BRIDGE_IP)
    lights = bridge.get_light_objects('id')
    num_lights = len(lights)

    # Set up an OpenAL context and stuff
    audio.initialize(num_lights)

    songs = os.environ[SONGS_PATH] if SONGS_PATH in os.environ else './songs'
    files = os.listdir(songs)
    for f in files:
        audio.add_file(f)

    while True:
        if queue is not None and not queue.empty():
            command = queue.get()
            if command['type'] == 'play':
                if command['file'] is None:
                    audio.play_random_sound()
                else:
                    audio.play_sound(command['file'].encode('ascii'))
            elif command['type'] == 'pause':
                audio.pause_sound()
            elif command['type'] == 'unpause':
                audio.unpause_sound()

        audio.update()

        # If we have audio playing currently, then loop through and ask
        # the audio module for each light's info
        if audio.is_audio_playing():
            for i in xrange(num_lights):
                light_info = audio.get_light(i)
                if light_info.set:
                    sys.stderr.write('-')
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
