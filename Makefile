.DEFAULT_GOAL := all

CXX := g++

CXXFLAGS := -Wall -std=c++23 -MP -MD \
	-I./libs/spdlog/include

LDFLAGS := -L./libs/spdlog/build -l:libspdlog.a

SRCDIR := src/
BINDIR := target/

SRCS := $(wildcard src/*.cpp)
OBJS := $(patsubst $(SRCDIR)%.cpp, $(BINDIR)%.o, $(SRCS))
DEPS := $(patsubst $(SRCDIR)%.cpp, $(BINDIR)%.d, $(SRCS))

-include Makefile.libs

all: libs ./target/game

./target/game: $(OBJS)
	$(CXX) -o $@ $^

$(BINDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

-include $(DEPS)

clean: $(OBJS) $(DEPS) libs/spdlog/build
	rm -r $^

.PHONY: all clean
