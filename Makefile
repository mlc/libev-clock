.PHONY: all
all: libev_clock

CXX = g++
OBJS = libev_clock.o
LIBS = -lev
CXXFLAGS = -O3 -march=native

libev_clock: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

clean:
	rm -f *~ *.o