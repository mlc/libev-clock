/* libev_clock.cc
 * mike castlmean, http://github.com/mlc/libev-clock
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include <iostream>
#include <locale>
#include <locale.h>
#include <ev++.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>

class Clock {
  ev::periodic clock;
  ev::sig sigwatch;
  ev::io stdinwatch;
  struct termios stored_termios;
  bool did_store_termios;

  void set_single_char(int fd) {
    struct termios st;
    tcgetattr(fd, &st);
    memcpy(&stored_termios, &st, sizeof(st));
    did_store_termios = true;
    st.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON);
    st.c_lflag &= ~(ECHO | ECHONL | ICANON);
    st.c_cflag &= ~CSIZE;
    st.c_cflag |= CS8;

    tcsetattr(fd, TCSANOW, &st);
  }

public:
  void foo(ev::periodic& w, int revents) {
    ev_tstamp ts = ev_now(EV_DEFAULT);
    time_t tt = (time_t) ts;
    char buf[256];
    struct tm tms;
    localtime_r(&tt, &tms);
    strftime(buf, 255, "%c", &tms);
    std::cout << buf /* << ' ' << std::fixed << ts */ << std::endl;
  }

  void stop() {
    ev_break(EV_DEFAULT_ EVBREAK_ALL);
  }

  void stop1(ev::sig &w, int revents) {
    std::cout << "goodbye!" << std::endl;
    stop();
  }

  void stop2(ev::io &w, int revents) {
    char buf[256];
    if (revents & ev::READ) { // drain STDIN if needed
      read(STDIN_FILENO, buf, 255);
    }
    stop();
  }

  Clock() : did_store_termios(false) {
    clock.set <Clock, &Clock::foo> (this);
    clock.set(0, 1);
    clock.start();

    sigwatch.set <Clock, &Clock::stop1> (this);
    sigwatch.start(SIGINT);

    set_single_char(STDIN_FILENO);
    stdinwatch.set <Clock, &Clock::stop2> (this);
    stdinwatch.start(STDIN_FILENO, ev::READ);
  }

  ~Clock() {
    if (did_store_termios) {
      tcsetattr(STDIN_FILENO, TCSANOW, &stored_termios);
    }
  }
};

int main(int argc, char** argv) {
  // ev_default_loop(EVBACKEND_SELECT);

  setlocale(LC_ALL, "");
  std::locale mylocale("");
  std::cout.imbue(mylocale);
  tzset();

  Clock * ps = new Clock();

  ev_loop(EV_DEFAULT_ 0);

  delete ps;

  return 0;
}
