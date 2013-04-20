from twitter import *
from phue import *
import sys
import config

class TwitterUserStream(TwitterStream):
    def __init__(self, *args, **kwargs):
        kwargs['domain'] = 'userstream.twitter.com'
        super(self.__class__, self).__init__(*args, **kwargs)


class Walter(object):
    @staticmethod
    def control_lights(phrase):
        pass

def main():
    twitter_stream = TwitterUserStream(auth=OAuth(**config.OAUTH))
    stream = twitter_stream.user(replies='all')

    for tweet in stream:
        if u'text' in tweet:
            Walter.control_lights(u'text')
            sys.stderr.write('%r\n' % tweet)

if __name__ == '__main__':
    main()
