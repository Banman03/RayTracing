CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -I src

TARGET = raytrace
SRC    = src/main.cpp

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) output.ppm

.PHONY: run clean
