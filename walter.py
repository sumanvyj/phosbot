from twitter import *
from phue import *
import config
import sys
import os
import re
import string
import time
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

def clean_filename(filename):
    fn = unicode("_".join(filename.lower().strip().split()))
    cleaned_filename = unicodedata.normalize('NFKD', fn).encode('ASCII', 'ignore')
    return ''.join(c for c in cleaned_filename if c in VALID_FILENAME_CHARS)

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

        if state.songcommand is not None and queue is not None:
            if state.songname is None:
                queue.put({
                    'type' : state.songcommand
                })
            else:
                song = state.songname.lower()
                songs = os.environ[SONGS_PATH] if SONGS_PATH in os.environ else './songs'
                files = os.listdir(songs)

                fuzzy_files = {}
                for f in files:
                    fn = '.'.join(f.split('.')[0:-1]).lower().translate(TABLE)
                    fuzzy_files[fn] = f

                if song in fuzzy_files:
                    song = fuzzy_files[song]

                    queue.put({
                        'type' : state.songcommand,
                        'file' : song
                    })

            return

        if state.volume is not None and queue is not None:
            queue.put({
                'type': 'volume',
                'vol' : state.volume
            })

            return

        if state.url is not None and queue is not None:
            # get title of YT video and sanitize
            # to be filename-worthy
            cmd = subprocess.Popen(['youtube-dl', '-e', state.url], stdout=subprocess.PIPE)
            out, err = cmd.communicate()
            filename = clean_filename(out) + '.ogg'

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

            queue.put({
                'type' : 'play',
                'file' : filename
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


def user_okay(user):
    return user in config.USER_WHITELIST


def main(phrase=None, queue=None):
    if phrase is not None:
        Walter.control_lights(phrase, queue)

    twitter_stream = TwitterUserStream(auth=OAuth(**config.OAUTH), block=False)
    stream = twitter_stream.user(replies='all')
    timestamp = time.time()

    for msg in stream:
        if time.time() - timestamp > 3.0:
            sys.stderr.write('.')
            timestamp = time.time()

        if msg is None:
            continue

        pprint.pprint(msg, stream=sys.stderr)

        if u'text' in msg:
            username = msg[u'screen_name']
            if not user_okay(username):
                sys.stderr.write('%r not in whitelist (%r)' % (username, config.USER_WHITELIST))
                continue

            phrase = msg[u'text']
            if USERNAME_RE.search(phrase) is None:
                continue

            phrase = USERNAME_RE.sub('', phrase)
            Walter.control_lights(phrase, queue)

        if u'direct_message' in msg:
            username = msg[u'direct_message'][u'sender_screen_name']
            if not user_okay(username):
                sys.stderr.write('%r not in whitelist (%r)' % (username, config.USER_WHITELIST))
                continue

            Walter.control_lights(msg[u'direct_message'][u'text'], queue)

if __name__ == '__main__':
    if len(sys.argv) > 1:
        main(phrase=sys.argv[1])
    else:
        main()
