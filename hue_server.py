from cxx import audio
from time import clock
from twitter import *
import sys
import config

# Set up an OpenAL context and stuff
audio.initialize()

audio.play_sound("click.ogg");

while clock() < 3:
  audio.update()

"""
auth = OAuth(
    config.OAUTH_TOKEN, config.OAUTH_SECRET,
    config.CONSUMER_KEY, config.CONSUMER_SECRET
)
"""
auth = UserPassAuth(config.USERNAME, config.PASSWORD)
twitter_stream = TwitterStream(auth=auth)
stream = twitter_stream.statuses.sample()

for tweet in stream:
    sys.stdout.write('%r\n' % tweet[u'text'])

# TODO implement the server part...

# Cleanup after audio stuffs
audio.deinitialize()
