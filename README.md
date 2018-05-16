SpeechOver
==========

Introduction
------------

Actually it's only some tests with the at-spi library. but the final target is to develop an Orca's alternative in gnu linux Accessibility.

Build
-----

To build SpeechOver you need to install
- glib
- libatspi
- speech-dispatcher
developement libraries.

For instence on Ubuntu:
$ sudo apt-get install libatspi2.0-dev libspeechd-dev libglib2.0-dev

The only working part for now is investigation/SpeechOver
$ cd investigation
$ gcc SpeechOver.cpp $(pkg-config --libs --cflags atspi-2 speech-dispatcher) -o SO -fpermissive

Usage
-----

To run, in the investigation directory
$ ./SO

Now there is onliy the posibility to use LEFT and RIGHT arrows keys to navigate ant UP and DOWN to interact or uninteract on the components
You can use SPACE to perform the default action when it exist.
