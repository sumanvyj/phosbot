
'''
The command you're interested in is process_command([cmd])
    param [cmd]:  the string command put into twitter
    return:  a StateChange object as defined below
'''


###################################
# define some statechange objects #
###################################

def enum(**enums):
    return type('Enum', (), enums)

# brightness
Bright = enum(ON='on', OFF='off', UP='up', DOWN='down')
# song volume
Volume = enum(LOUD='loud', SOFT='soft', HIGH='high', LOW='low')


class StateChange:
    '''
    StateChange.bright
        the brightness change.  {Bright.ON, Bright.OFF, Bright.UP, Bright.DOWN}
        if None, no change of state.
    StateChange.name
        the name of the light to target.  {some light name}
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


    def __init__():
        self.bright = None
        self.name = None
        self.color = None
        self.song = None
        self.volume = None






#########################
# parse those commands! #
#########################

def process_command(cmd):
    state = StateChange()
    cmd = "Turn on lights"
    state = parse_bright(cmd, state)
    return state


def parse_bright(cmd, state):
    if('on' in cmd):
        state.bright = Bright.ON
    return state

def parse_name(cmd, state):
    if('light' in cmd):
        state.name = None
    return state

