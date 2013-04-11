This will be a snazzy server thingy that controls hue lights and stuff.

## Setting up on Mac OS X

Using Homebrew, first make sure all these packages are installed:

    brew install boost libogg libvorbis sdl cmake

Install `pip`:

    sudo easy_install pip

Install `virtualenv`:

    sudo pip install virtualenv

Make a virtualenv called `phosbot`:

    virtualenv phosbot

Activate the virtualenv:

    source ./phosbot/bin/activate

Install all the required packages:

    pip install -r requirements.txt

CMAke the Makefile:

    cmake .

Actually `make`:

    make

That should be it.
