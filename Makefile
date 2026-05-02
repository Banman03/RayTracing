CXX      = g++
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LIBS   := $(shell sdl2-config --libs)
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -I src -I . $(SDL_CFLAGS)

TARGET = raytrace
SRC    = src/main.cpp

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(SDL_LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) output.ppm

.PHONY: run clean
