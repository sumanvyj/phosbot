from twitter import *
from phue import *
import config
import sys
import os
import re
import string
import colorsys
import speech.commandparser
import pprint
import subprocess
import unicodedata

class TwitterUserStream(TwitterStream):
    def __init__(self, *args, **kwargs):
        kwargs['domain'] = 'userstream.twitter.com'
        super(self.__class__, self).__init__(*args, **kwargs)


USERNAME_RE = re.compile('^%s' % config.USERNAME, re.IGNORECASE)
MAX_COLOR = 255.0
MAX_HUE = 65535
MAX_SATURATION = 254
MAX_BRIGHTNESS = 254
SONGS_PATH = 'SONGS_PATH'
TABLE = string.maketrans('-_', '  ')
VALID_FILENAME_CHARS = "-_.() %s%s" % (string.ascii_letters, string.digits)

def cleanFilename(filename):
    filename = unicode("_".join(filename.strip().split()))
    cleanedFilename = unicodedata.normalize('NFKD', filename).encode('ASCII', 'ignore')
    return ''.join(c for c in cleanedFilename if c in VALID_FILENAME_CHARS)

class Walter(object):
    _bridge = None

    @classmethod
    def bridge(cls):
        if cls._bridge is None:
            cls._bridge = Bridge(config.BRIDGE_IP)

        return cls._bridge

    @classmethod
    def control_lights(cls, phrase, queue=None):
        bridge = cls.bridge()
        phrase = phrase.strip()
        light_names = [light.name for light in bridge.lights]
        state = speech.commandparser.process_command(phrase, light_names)
        names = light_names

        if state.names:
            fuzzy_names = {}
            for light in light_names:
                fuzzy_names[light.lower()] = light

            names = [fuzzy_names[name] for name in state.names if name in fuzzy_names]

        sys.stderr.write('names=%r\n%r\n' % (names, state,))

        command = {}

        if state.url is not None:
            # get title of YT video and sanitize
            # to be filename-worthy
            cmd = subprocess.Popen(['youtube-dl', '-e', state.url], stdout=subprocess.PIPE)
            out, err = cmd.communicate()
            filename = cleanFilename(out) + '.ogg'

            # get youtube video descriptor string
            cmd = subprocess.Popen(['youtube-dl', '--get-filename', state.url], stdout=subprocess.PIPE)
            out, err = cmd.communicate()
            yt_desc = out.strip().split('.')[0]

            songs = os.environ[SONGS_PATH] if SONGS_PATH in os.environ else './songs'

            old_dir = os.getcwd()
            os.chdir(songs)
            files = os.listdir('.')
            if filename not in files:
                subprocess.call(['youtube-dl', '-xf', '43', state.url])
                os.rename(yt_desc + '.ogg', filename)

            os.chdir(old_dir)

            if queue is not None:
                queue.put({
                    'type' : 'play',
                    'file' : filename
                })

            return

        if state.song is not None:
            song = state.song.lower()
            songs = os.environ[SONGS_PATH] if SONGS_PATH in os.environ else './songs'
            files = os.listdir(songs)

            fuzzy_files = {}
            for f in files:
                fn = f.split('.')[0:-1].join('.').lower().translate(TABLE)
                fuzzy_files[fn] = f

            if song in fuzzy_files:
                song = fuzzy_files[song]
                if queue is not None:
                    queue.put({
                        'type' : 'play',
                        'file' : song
                    })

            return

        if state.power is not None:
            command['on'] = state.power

        if state.setlight is not None:
            command['bri'] = int(MAX_BRIGHTNESS * (state.setlight / 100.0))

        if state.fadetime is not None:
            command['on'] = state.fadetime >= 0
            command['transitiontime'] = abs(state.fadetime)

        if state.color is not None:
            r, g, b = state.color
            r, g, b = r / MAX_COLOR, g / MAX_COLOR, b / MAX_COLOR
            h, s, v = colorsys.rgb_to_hsv(r, g, b)
            command['hue'] = int(MAX_HUE * h)
            command['sat'] = int(MAX_SATURATION * s)

        if command:
            bridge.set_light(names, command)
            pprint.pprint(command, stream=sys.stderr)

        if state.changelight is not None:
            for light in bridge.lights:
                if light.name not in names:
                    continue

                delta = int(MAX_BRIGHTNESS * (state.changelight / 100.0))
                light.brightness += delta

        return state


def main(phrase=None, queue=None):
    if phrase is not None:
        Walter.control_lights(phrase, queue)
        return

    twitter_stream = TwitterUserStream(auth=OAuth(**config.OAUTH))
    stream = twitter_stream.user(replies='all')

    for msg in stream:
        pprint.pprint(msg, stream=sys.stderr)

        if u'text' in msg:
            phrase = msg[u'text']
            if USERNAME_RE.search(phrase) is None:
                continue

            phrase = USERNAME_RE.sub('', phrase)
            Walter.control_lights(phrase, queue)
        if u'direct_message' in msg:
            Walter.control_lights(msg[u'direct_message'][u'text'], queue)

if __name__ == '__main__':
    if len(sys.argv) > 1:
        main(phrase=sys.argv[1])
    else:
        main()
