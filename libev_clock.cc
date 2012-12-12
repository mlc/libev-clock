#include <iostream>
#include <locale>
#include <locale.h>
#include <ev++.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>

class Silly {
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
    clock.stop();
    sigwatch.stop();
    stdinwatch.stop();
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

  Silly() : did_store_termios(false) {
    clock.set <Silly, &Silly::foo> (this);
    clock.set(0, 1);
    clock.start();

    sigwatch.set <Silly, &Silly::stop1> (this);
    sigwatch.start(SIGINT);

    set_single_char(STDIN_FILENO);
    stdinwatch.set <Silly, &Silly::stop2> (this);
    stdinwatch.start(STDIN_FILENO, ev::READ);
  }

  ~Silly() {
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

  Silly * ps = new Silly();

  ev_loop(EV_DEFAULT_ 0);

  delete ps;

  return 0;
}