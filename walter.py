from twitter import *
from phue import *
import sys
import config
import speech.commandparser
import re

class TwitterUserStream(TwitterStream):
    def __init__(self, *args, **kwargs):
        kwargs['domain'] = 'userstream.twitter.com'
        super(self.__class__, self).__init__(*args, **kwargs)


class Walter(object):
    _bridge = None
    _username_re = re.compile('^%s' % config.USERNAME, re.IGNORECASE)

    @classmethod
    def bridge(cls):
        if cls._bridge is None:
            cls._bridge = Bridge(config.BRIDGE_IP)

        return cls._bridge

    MAX_BRIGHTNESS = 254
    MAX_COLOR = 255.0
    MAX_HUE = 65535
    MAX_SATURATION = 254

    @classmethod
    def control_lights(cls, phrase):
        if cls._username_re.search(phrase) is None:
            return None

        b = cls.bridge()
        phrase = cls._username_re.sub('', phrase).strip()

        state = speech.commandparser.process_command(phrase)

        names = state.names if state.names else [x.name for x in b.lights]
        command = {}

        if state.power is not None:
            command['on'] = state.power

        if state.setlight is not None:
            command['brightness'] = int(cls.MAX_BRIGHTNESS * (state.setlight / 100.0))

        if state.color is not None:
            r, g, b = state.color
            r, g, b = r / MAX_COLOR, g / MAX_COLOR, b / MAX_COLOR
            h, s, v = colorsys.rgb_to_hsv(r, g, b)
            command['hue'] = int(MAX_HUE * h)
            command['sat'] = int(MAX_SATURATION * s)


        if state.changelight is not None:
            for light in (for x in b.lights if x.name in names):
                factor = (100 + state.changelight) / 100.0
                light.brightness = int(light.brightness * factor)
        else:
            b.set_light(names, command)

        return state


def main():
    twitter_stream = TwitterUserStream(auth=OAuth(**config.OAUTH))
    stream = twitter_stream.user(replies='all')

    for tweet in stream:
        if u'text' in tweet:
            Walter.control_lights(tweet[u'text'])
            sys.stderr.write('%r\n' % tweet)

if __name__ == '__main__':
    main()
