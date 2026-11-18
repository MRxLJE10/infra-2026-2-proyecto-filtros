CXX      ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -fopenmp
BIN      := motor comparar generar arnes

all: $(BIN)

motor: src/main.cpp src/filtros.cpp src/imagen.cpp
	$(CXX) $(CXXFLAGS) -Isrc -o $@ $^

comparar: tests/comparar.cpp
	$(CXX) $(CXXFLAGS) -Isrc -o $@ $^

generar: bench/generar.cpp src/imagen.cpp
	$(CXX) $(CXXFLAGS) -Isrc -o $@ $^

arnes: bench/arnes.cpp src/filtros.cpp src/imagen.cpp
	$(CXX) $(CXXFLAGS) -Isrc -o $@ $^

test_sobel: tests/test_sobel.cpp src/filtros.cpp src/imagen.cpp
	$(CXX) $(CXXFLAGS) -Isrc -o $@ $^

pruebas: all test_sobel
	bash tests/pruebas.sh
	./test_sobel

clean:
	rm -f $(BIN) test_sobel

.PHONY: all pruebas clean
