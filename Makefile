.DEFAULT_GOAL := all

CXX := g++

CXXFLAGS := -Wall -std=c++23 -MP -MD \
	-I./libs/spdlog/include \
	-I./libs/raylib/src \
	-I./include

LDFLAGS := -L./libs/spdlog/build -l:libspdlog.a \
	-L./libs/raylib/src -l:libraylib.a

SRCDIR := src/
BINDIR := target/

SRCS := $(wildcard src/*.cpp)
OBJS := $(patsubst $(SRCDIR)%.cpp, $(BINDIR)%.o, $(SRCS))
DEPS := $(patsubst $(SRCDIR)%.cpp, $(BINDIR)%.d, $(SRCS))

-include Makefile.libs

all: libs ./target/game

./target/game: $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(BINDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

-include $(DEPS)

clean: $(OBJS) $(DEPS) libs/spdlog/build libs/raylib/src/libraylib.a
	rm -r $^

.PHONY: all clean
