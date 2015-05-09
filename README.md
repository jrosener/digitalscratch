Digital-Scratch
--------------

### A vinyl turntable emulation software for DJ

[Digital-Scratch](http://www.digital-scratch.org/ "Digital-scratch") is a free software for DJs. It analyzes motions (rotation speed and direction) of a vinyl disc played by a turntable. It also provide a nice and simple player that plays digital tracks allowing you to mix as you would do it with regular records.  

__In other words__: with Digital-Scratch you can mix your digital file collection with regular vinyl turntables.

* More informations at [http://www.digital-scratch.org](http://www.digital-scratch.org).
* Mailing list: digitalscratch@googlegroups.com (to subscribe, send a mail to digitalscratch+subscribe@googlegroups.com)

### Requirements

* 1 or 2 timecoded vinyl disc, ([Serato Scratch Live](http://serato.com/scratchlive), [Final Scratch](http://en.wikipedia.org/wiki/Final_Scratch),...)
* 1 or 2 vinyl turntables and a mixer
* A GNU/Linux or MS Windows computer
* A sound card driven by [JACK](http://jackaudio.org/) providing 1 or 2 stereo inputs

### Software architecture

The core motion detection is done by an SDK built as a shared library: _libdigitalscratch_. It can be integrated with various kind of controlable software player.

There is also a player which uses the library: _digitalscratch_. This player is able to play one or two tracks. The playback parameters (speed and direction) could be changed in real time. It also provides a file explorer used to select tracks to play.

### Software dependencies

_libdigitalscratch_ (motion detection) is based on:
* [Qt](http://www.qt.io/)

_digitalscratch_ (audio player) is based on:
* [Qt](http://www.qt.io/)
* [LibAV](http://libav.org)
* [Sample rate library](http://www.mega-nerd.com/SRC/)
* [JACK](http://jackaudio.org/)
* [LibKeyFinder](http://www.ibrahimshaath.co.uk/keyfinder/)

Install
--------------

### Ubuntu
    sudo add-apt-repository ppa:julien-rosener/digitalscratch
    sudo apt-get update
    sudo apt-get install digitalscratch

### Debian 8
    wget -qO - http://www.digital-scratch.org/debian/julien.rosener@digital-scratch.org.gpg.key | sudo apt-key add -
    sudo add-apt-repository "deb http://www.digital-scratch.org/debian/ stable main"
    sudo apt-get update
    sudo apt-get install digitalscratch

### MS Windows
Go to the download page: [http://www.digital-scratch.org/download.html](http://www.digital-scratch.org/download.html)

Build
--------------

### GNU/Linux

#### Install build tools (Ubuntu 15.04 - Vivid)
    sudo apt-add-repository ppa:julien-rosener/digitalscratch
    sudo apt-get update
    sudo apt-get install build-essential qt5-default libjack-jackd2-dev libsamplerate0-dev libkeyfinder-dev libavformat-dev libavcodec-dev libavutil-dev qtmultimedia5-dev libqt5multimedia5-plugins
    
#### Install build tools (Debian 8 - Jessie)
    wget -qO - http://www.digital-scratch.org/debian/julien.rosener@digital-scratch.org.gpg.key | sudo apt-key add -
    sudo add-apt-repository "deb http://www.digital-scratch.org/debian/ stable main"
    sudo apt-get update
    sudo apt-get install build-essential qt5-default libjack-jackd2-dev libsamplerate0-dev libkeyfinder-dev qtmultimedia5-dev libavformat-dev libavcodec-dev libavutil-dev

#### Compile and install _libdigitalscratch_
    cd libdigitalscratch
    qmake
    make
    sudo make install

#### Compile and install _digitalscratch_ player
    cd ../digitalscratch
    qmake
    make
    sudo make install
    
### MS Windows

#### Install build environment
* Install [Visual Studio 2013 Community](https://www.visualstudio.com/en-us/products/visual-studio-community-vs.aspx)
* Install [Qt 5.x for Windows 32-bit (MSVC 2013, OpenGL), including QtCreator](http://www.qt.io/download-open-source/)
    
#### Compile _libdigitalscratch_ and _digitalscratch_ player
* Start Qt Creator.
* Load project `libdigitalscratch.pro` and `digitalscratch.pro`.
* For each projects, use the build kit which uses the compiler from Visual Studio 2013 Community.
* For `libdigitalscratch` set the output directory to `libdigitalscratch\` (Debug and Release build configuration).
* For `digitalscratch` set the output directory to `digitalscratch\` (Debug and Release build configuration).
* For both projects, right click on the project name, then "Run qmake", then "build".

License
--------------

Digital-Scratch player and library are released under [GNU GPL](http://www.gnu.org/copyleft/gpl.html) license.
