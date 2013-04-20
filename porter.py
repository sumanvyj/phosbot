from cxx import audio
from time import clock
from phue import *
import config

def main(queue=None):
    #b = Bridge(config.BRIDGE_IP)
    #b.connect()
    #b.get_api()

    #lights = b.get_light_objects('id')
    #num_lights = len(lights)
    num_lights = 2

    # Set up an OpenAL context and stuff
    audio.initialize(num_lights)

    #audio.add_file("click.ogg");
    #audio.add_file("calm1.ogg");

    audio.play_sound("click.ogg");

    while audio.is_audio_playing():
      audio.update()

    # TODO implement the server part...

    # Cleanup after audio stuffs
    audio.deinitialize()

if __name__ == '__main__':
    main()
