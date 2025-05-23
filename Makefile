.DEFAULT_GOAL := all

CXX := g++

CXXFLAGS := -Wall -std=c++23 -MP -MD \
		-I./libs/spdlog/include

LDFLAGS := -L./libs/spdlog/build -l:libspdlog.a

SRCS := $(shell find ./src/ -type f -name '*.cpp')
OBJS := $(patsubst %.cpp,%.o,$(SRCS))
DEPS := $(patsubst %.cpp,%.d,$(SRCS))

-include Makefile.libs

all: libs game

game: $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS)

%.o: %.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

-include $(DEPS)

clean: $(OBJS) $(DEPS) libs/spdlog/build
	rm -r $^

.PHONY: all clean
