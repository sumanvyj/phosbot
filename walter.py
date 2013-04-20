from twitter import *
from phue import *
import config
import sys
import pprint
import re
import colorsys
import speech.commandparser

class TwitterUserStream(TwitterStream):
    def __init__(self, *args, **kwargs):
        kwargs['domain'] = 'userstream.twitter.com'
        super(self.__class__, self).__init__(*args, **kwargs)


USERNAME_RE = re.compile('^%s' % config.USERNAME, re.IGNORECASE)
MAX_BRIGHTNESS = 254
MAX_COLOR = 255.0
MAX_HUE = 65535
MAX_SATURATION = 254


class Walter(object):
    _bridge = None

    @classmethod
    def bridge(cls):
        if cls._bridge is None:
            cls._bridge = Bridge(config.BRIDGE_IP)

        return cls._bridge

    @classmethod
    def control_lights(cls, phrase):
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

        if state.power is not None:
            command['on'] = state.power

        if state.setlight is not None:
            command['bri'] = int(MAX_BRIGHTNESS * (state.setlight / 100.0))

        if state.color is not None:
            r, g, b = state.color
            r, g, b = r / MAX_COLOR, g / MAX_COLOR, b / MAX_COLOR
            h, s, v = colorsys.rgb_to_hsv(r, g, b)
            command['hue'] = int(MAX_HUE * h)
            command['sat'] = int(MAX_SATURATION * s)

        if state.changelight is not None:
            for light in bridge.lights:
                if light.name not in names:
                    continue

                factor = (100 + state.changelight) / 100.0
                light.brightness = int(light.brightness * factor)

        bridge.set_light(names, command)

        return state


def main():
    if len(sys.argv) > 1:
        phrase = sys.argv[1]
        Walter.control_lights(phrase)

    twitter_stream = TwitterUserStream(auth=OAuth(**config.OAUTH))
    stream = twitter_stream.user(replies='all')

    for msg in stream:
        pprint.pprint(msg, stream=sys.stderr)

        if u'text' in msg:
            phrase = msg[u'text']
            if USERNAME_RE.search(phrase) is None:
                continue

            phrase = USERNAME_RE.sub('', phrase)
            Walter.control_lights(phrase)
        if u'direct_message' in msg:
            Walter.control_lights(msg[u'direct_message'][u'text'])

if __name__ == '__main__':
    main()
