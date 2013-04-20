
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

# on/off power
Power = _enum(ON='on', OFF='off')
# absolute brightness
SetLight = _enum(BRIGHT='bright', DIM='dim', HALF='half', FULL='full')
# relative brightness
ChangeLight = _enum(UP='up', DOWN='down', BRIGHTER='brighter', DIMMER='dimmer')
# a fade time
FadeTime = _enum(FADE='fade', IN='in', OUT='out', 
                 FAST='fast', QUICKLY='quickly',
                 SLOW='slow', SLOWLY='slowly', SNAP='snap')
# song volume
Volume = _enum(LOUD='loud', SOFT='soft', HIGH='high', LOW='low')

# extra modifiers
Extra = _enum(REALLY='really', VERY='very')


class StateChange(object):
    '''
    StateChange.power
        boolean on / off {true/false}, true being on
        if None, no change of state.
    StateChange.setlight
        absolute brightness setting {a 0-100 percentage brightness set}
        if None, no change of state.
    StateChange.changelight
        relative brightness change.  {a 0-100 percentage brightness change}
        if None, no change of state.
    StateChange.fadetime
        a fade time in seconds.  {a time in 1/10ths seconds}
        negative time values indicate fade out, positive values fade in
        if None, default fade time.  if 0, instant change
    StateChange.names
        the names of the lights to target.  {a list of light names}
        if None, change all lights.
    StateChange.color
        the color to change to.  {some rgb (r,g,b) color}
        if None, no color change.
    StateChange.song
        the song name to play.  {some song name}
        if None, no song to play
    StateChange.volume
        volume to play at.  {Volume.LOUD, Volume.SOFT, Volume.HIGH, Volume.LOW}
        if None, play at normal volume
    '''

    __slots__ = (
        'power', 'setlight', 'changelight', 'fadetime',
        'names', 'color', 'song', 'volume'
    )

    def __init__(self, **kwargs):
        self.power = kwargs.get('power')
        self.setlight = kwargs.get('setlight')
        self.changelight = kwargs.get('changelight')
        self.fadetime = kwargs.get('fadetime')
        self.names = kwargs.get('names')
        self.color = kwargs.get('color')
        self.song = kwargs.get('song')
        self.volume = kwargs.get('volume')

    def __repr__(self):
        return self.__str__()

    def __str__(self):
        return '%s(power=%r, setlight=%r, changelight=%r, fadetime=%r, names=%r, color=%r, song=%r, volume=%r)' % (
            self.__class__.__name__,
            self.power, self.setlight, self.changelight, self.fadetime,
            self.names, self.color, self.song, self.volume
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
    return Extra.REALLY in cmd or Extra.VERY in cmd

def parse_power(cmd, state):
    val = None
    if _match(cmd, Power.ON):
        val = True
    if _match(cmd, Power.OFF):
        val = False
    state.power = val
    return (cmd, state)


def parse_setlight(cmd, state):
    val = 0
    extra = _hasextra(cmd)

    # no elifs because someone may say "Turn on lights to dim"
    # more of a key-word search
    if _match(cmd, SetLight.FULL) or _match(cmd, SetLight.BRIGHT):
        val = 100
    if _match(cmd, SetLight.HALF):
        val = 50
    if _match(cmd, SetLight.DIM):
        val = 30
        val /= 2 if extra else 1

    state.setlight = val
    return (cmd, state)

def parse_changelight(cmd, state):
    val = 0
    extra = _hasextra(cmd)

    # no elifs because someone may say "Turn down lights to dimmer"
    # more of a key-word search
    if _match(cmd, ChangeLight.UP) or _match(cmd, ChangeLight.BRIGHTER):
        val = 40
    if _match(cmd, ChangeLight.DOWN) or _match(cmd, ChangeLight.DIMMER):
        val = -40

    val *= 2 if extra else 1
    state.changelight = val
    return (cmd, state)

def parse_fadetime(cmd, state):
    val = None
    extra = _hasextra(cmd)

    if _match(cmd, FadeTime.SNAP):
        val = 0

    ########### 
    # REMEMBER:  VALUES ARE 10ths OF SECONDS
    ########### 
    try:
        index = cmd.index(FadeTime.FADE)
        # if no 'fade', will Error and drop to bottom
        print "got it"

        # default fade time of 1 second
        val = 10
        if _match(cmd, FadeTime.SLOW) or _match(cmd, FadeTime.SLOWLY):
            val = 50
            val *= 2 if extra else 1

        # find out if its 'in' or 'out'
        token = cmd[index+1]
        if token == FadeTime.IN:
            # positive value
            _match(cmd, FadeTime.IN)

        elif token == FadeTime.OUT:
            # negative value
            val *= -1
            _match(cmd, FadeTime.OUT)

    except ValueError:
        pass

    # catch the case of simply 'turn quickly to red'
    if _match(cmd, FadeTime.FAST) or _match(cmd, FadeTime.QUICKLY):
        # instantly, or in one 10th of a second
        val = 0 if extra else 1

    state.fadetime = val
    return (cmd, state)


def parse_names(cmd, state, names):
    if _match(cmd, 'lights') or _match(cmd, 'all'):
        state.names = None

    names = tuple(set(cmd) & set(names))

    if len(names) != 0:
        cmd = [w for w in cmd if w not in names]
        state.names = names

    return (cmd, state)


def parse_color(cmd, state):
    for word in cmd:
        try:
            state.color = webcolors.name_to_rgb(word)
            cmd.remove(word)
            break
        except ValueError:
            continue
    return (cmd, state)


def parse_song(cmd, state):
    return (cmd, state)



def parse_eastereggs(cmd, state):
    if _match(cmd, 'let there be light'):
        state.power = True
    if _match(cmd, 'sleep'):
        state.power = False
    if _match(cmd, 'go to hell'):
        state.color = webcolors.name_to_rgb('red')
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
    cmd, state = parse_setlight(cmd, state)
    cmd, state = parse_changelight(cmd, state)
    cmd, state = parse_fadetime(cmd, state)
    cmd, state = parse_names(cmd, state, names)
    cmd, state = parse_color(cmd, state)
    cmd, state = parse_song(cmd, state)
    cmd, state = parse_eastereggs(cmd, state)
    return state
