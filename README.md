Digital-Scratch
--------------

### A vinyl turntable emulation software

[Digital-Scratch](http://www.digital-scratch.org/ "Digital-scratch") is a free software that analyze motions (rotation speed and direction) of a vinyl disc played by a turntable.  
It also provide a nice and simple player that play digital tracks allowing you to mix as you would do it with regular records.

More informations on [http://www.digital-scratch.org](http://www.digital-scratch.org).

### Requirements

* 1 or 2 timecoded vinyl disc, ([Serato Scrath Live](http://serato.com/scratchlive), [Final Scratch](http://en.wikipedia.org/wiki/Final_Scratch),...)
* 1 or 2 vinyl turntables
* A GNU/Linux computer
* A sound card driven by [JACK](http://jackaudio.org/) providing 1 or 2 stereo inputs

### Architecture

The core motion detection is done by an SDK built as a shared library: _libdigitalscratch_. It can be integrated with various kind of controllable software player.

Digital-Scratch also provides a player which uses the library: _digitalscratch_. This player is able to play one or two tracks. The playback parameters (speed and direction) could be changed in real time. It also provides a simplistic file explorer used to select tracks to play.

### Software dependencies

_libdigitalscratch_ (motion detection) is based on:
* [Qt 4.x](http://qt.nokia.com/)

_digitalscratch_ (audio player) is based on:
* [Qt 4.x](http://qt.nokia.com/)
* [mpg123](http://www.mpg123.de/)
* [FLAC](http://flac.sourceforge.net/)
* [Sample rate library](http://www.mega-nerd.com/SRC/)
* [JACK](http://jackaudio.org/)

Build
--------------

### Ubuntu 12.10 (quantal)
    sudo apt-get install build-essential qt4-sdk libmpg123-dev libjack-jackd2-dev libsamplerate0-dev libflac-dev
    qmake
    make
    sudo make install

Package install
--------------

### Ubuntu 12.10 (quantal)
    sudo add-apt-repository ppa:julien-rosener/digitalscratch
    sudo apt-get update
    sudo apt-get install digitalscratch

License
--------------

Digital-Scratch player and library are released under [GNU GPL](http://www.gnu.org/copyleft/gpl.html) license.
