
'''
The command you're interested in is process_command([cmd])
    param [cmd]:  the string command put into twitter
    param optional [names]:  a list of light bulb names
    return:  a StateChange object as defined below
'''

import webcolors

###################################
# define some statechange objects #
###################################

def _enum(**enums):
    return type('Enum', (), enums)

# absolute brightness
Power = _enum(ON='on', OFF='off', BRIGHT='bright', DIM='dim')
# relative brightness
Bright = _enum(UP='up', DOWN='down', BRIGHTER='brighter', DIMMER='dimmer')
# song volume
Volume = _enum(LOUD='loud', SOFT='soft', HIGH='high', LOW='low')


class StateChange(object):
    '''
    StateChange.power
        absolute brightness setting {a percentage brightness set}
        if None, no change of state.
    StateChange.brightness
        relative brightness change.  {a percentage brightness change}
        if None, no change of state.
    StateChange.names
        the names of the lights to target.  {a list of light names}
        if None, change all lights.
    StateChange.color
        the color to change to.  {some #ffffff rgb hex color}
        if None, no color change.
    StateChange.song
        the song name to play.  {some song name}
        if None, no song to play
    StateChange.volume
        volume to play at.  {Volume.LOUD, Volume.SOFT, Volume.HIGH, Volume.LOW}
        if None, play at normal volume
    '''

    __slots__ = ('power', 'brightness', 'names', 'color', 'song', 'volume')

    def __init__(self, **kwargs):
        self.power = kwargs.get('power')
        self.brightness = kwargs.get('brightness')
        self.names = kwargs.get('names')
        self.color = kwargs.get('color')
        self.song = kwargs.get('song')
        self.volume = kwargs.get('volume')

    def __repr__(self):
        return self.__str__()

    def __str__(self):
        return '%s(power=%r, brightness=%r, target=%r, color=%r, song=%r, volume=%r)' % (
            self.__class__.__name__,
            self.power, self.brightness, self.names,
            self.color, self.song, self.volume
        )


#########################
# parse those commands! #
#########################


def _match(cmd, key):
    if key in cmd:
        cmd.remove(key)
        return True

    return False

def _hasextra(cmd):
    return 'really' in cmd or 'very' in cmd

def parse_power(cmd, state):
    val = 0
    extra = _hasextra(cmd)

    # no elifs because someone may say "Turn on lights to dim"
    # more of a key-word search
    if _match(cmd, Power.ON) or _match(cmd, Power.BRIGHT):
        val = 100
    if _match(cmd, Power.DIM):
        val = 30
        val /= 2 if extra else val
    if _match(cmd, Power.OFF):
        val = 0

    state.power = val
    return (cmd, state)

def parse_brightness(cmd, state):
    val = 0
    extra = _hasextra(cmd)

    # no elifs because someone may say "Turn down lights to dimmer"
    # more of a key-word search
    if _match(cmd, Bright.UP) or _match(cmd, Bright.BRIGHTER):
        val = 40
    if _match(cmd, Bright.DOWN) or _match(cmd, Bright.DIMMER):
        val = -40

    val *= 2 if extra else val
    state.brightness = val
    return (cmd, state)

def parse_names(cmd, state, names):
    if _match(cmd, 'lights') or _match(cmd, 'all'):
        state.names = None

    names = tuple(set(cmd) & set(names))

    if len(names) != 0:
        cmd = [w for w in cmd if w in names]
        state.names = names

    return (cmd, state)

def parse_color(cmd, state):
    for word in cmd:
        try:
            state.color = webcolors.name_to_hex(word, spec='css3')
            cmd.remove(word)
            break
        except ValueError:
            continue
    return (cmd, state)

def parse_eastereggs(cmd, state):
    return (cmd, state)



################################
# the actual command processor #
################################

def process_command(cmd, names=list()):
    '''
    takes a string command and parses commands out of it
    (optional) a list of lightbulb names
    returns a StateChange object, or None if not a valid command
    '''
    state = StateChange()
    cmd = cmd.lower().split()
    names = [n.lower() for n in names]
    _match(cmd, 'turn')

    cmd, state = parse_power(cmd, state)
    cmd, state = parse_brightness(cmd, state)
    cmd, state = parse_names(cmd, state, names)
    cmd, state = parse_color(cmd, state)
    cmd, state = parse_eastereggs(cmd, state)
    return state
