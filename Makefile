CXX = g++
CFLAGS += -c -Wall -Werror -std=c++17
LDFLAGS += -lX11

SOURCES = cb_handlers.cpp selection.cpp cbread.cpp
OBJECTS=$(SOURCES:.cpp=.o)

all: $(SOURCES) cbread check

cbread: $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o cbread

.cpp.o:
	$(CXX) $(CFLAGS) $< -o $@

check: check.o
	$(CXX) check.o $(LDFLAGS) -o check

check.o:
	$(CXX) $(CFLAGS) check.cpp -o check.o

clean:
	rm -rf *.o cbread check
