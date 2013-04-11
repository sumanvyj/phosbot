This will be a snazzy server thingy that controls hue lights and stuff.

## Setting up on Mac OS X

Using Homebrew, first make sure all these packages are installed:

    brew install python boost libogg libvorbis sdl cmake

Make sure the path is set up correctly in your `~/.bashrc`:

    PATH=${PATH}:/usr/local/share/python

Install `virtualenv`:

    pip install virtualenv

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
