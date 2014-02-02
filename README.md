# PCF8583

PCF8583 Real Time Clock and Event Counter Library for Arduino

You can read about this library in my blog post about [Counting events with Arduino and PCF8583][1].

## Whys

### Why is your folders structure the way it is?

I use [Ino Tookit][2] to build my code. This tool enforces you to use a given structure for your projects,
the same way Arduino IDE does (the folder has to be named after the main .ino file).
So to use Ino while keeping compatibility with the Arduino way of publishing libraries I use this structure:

* root
    * COPYING 
    * README.md
    * code
        * src
            * .empty
        * lib
            * PCF8583
                * PCF8583.cpp
                * PCF8583.h
                * examples
                    * tests
                        * tests.ino


So I just symlink code/lib/PCF8583 folder from inside the Arduino libraries folder and the library and examples come available from the Arduino IDE.
To test the examples using Ino I symlink each example  to code/src (which is empty) and just do a "ino build".

### Why GPLv3?

I'm not very much into licenses, I just want my code to be freely available, just like the code I've learnt from.
GPLv3 is the license I've been using lately.

[1]: http://tinkerman.eldiariblau.net/counting-events-with-arduino-and-pcf8583
[2]: http://www.inotool.org
