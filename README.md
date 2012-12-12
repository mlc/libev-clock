libev-clock
===========

This is a simple C++ command-line clock based on [libev][]. It was
mainly written as an excuse to play with libev.

You can use, distribute, and modify it under the terms of the [WTFPL][].

To compile, make sure you have libev installed (`sudo aptitude install
libev-dev`), then just run `make`. If you have an operating system
under which you need to do something else, let me know.

Then, just run `./libev_clock`, and the time will be printed every
second, as close to the top of the second as possible. Press ^C or any
other key to quit.

[libev]: http://software.schmorp.de/pkg/libev.html
[WTFPL]: http://sam.zoy.org/wtfpl/ "Do What The Fuck You Want To Public License"
