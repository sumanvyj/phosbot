from twitter import *
from phue import *
import sys
import config

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
    if u'text' in tweet:
        sys.stdout.write('%r\n' % tweet[u'text'])
