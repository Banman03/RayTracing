CXX        := g++
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LIBS   := $(shell sdl2-config --libs)

COMMON_FLAGS := -std=c++17 -O2 -Wall -Wextra $(SDL_CFLAGS)
RT_FLAGS     := $(COMMON_FLAGS) -I src -I .
PT_FLAGS     := $(COMMON_FLAGS) -I src -I pathTracing

RT_TARGET := raytrace
PT_TARGET := pathtrace

RT_SRC := src/main.cpp
PT_SRC := pathTracing/main.cpp

all: $(RT_TARGET) $(PT_TARGET)

$(RT_TARGET): $(RT_SRC)
	$(CXX) $(RT_FLAGS) -o $@ $^ $(SDL_LIBS)

$(PT_TARGET): $(PT_SRC)
	$(CXX) $(PT_FLAGS) -o $@ $^ $(SDL_LIBS)

run: $(RT_TARGET)
	./$(RT_TARGET)

run-pt: $(PT_TARGET)
	./$(PT_TARGET)

clean:
	rm -f $(RT_TARGET) $(PT_TARGET) output.ppm

.PHONY: all run run-pt clean
