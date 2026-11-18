CXX      ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -fopenmp
BIN      := motor comparar generar arnes comparar_histogramas comparar_false_sharing experimento_false_sharing

all: $(BIN)

motor: src/main.cpp src/filtros.cpp src/histograma.cpp src/imagen.cpp
	$(CXX) $(CXXFLAGS) -Isrc -o $@ $^

comparar: tests/comparar.cpp
	$(CXX) $(CXXFLAGS) -Isrc -o $@ $^

generar: bench/generar.cpp src/imagen.cpp
	$(CXX) $(CXXFLAGS) -Isrc -o $@ $^

arnes: bench/arnes.cpp src/filtros.cpp src/histograma.cpp src/imagen.cpp
	$(CXX) $(CXXFLAGS) -Isrc -o $@ $^

comparar_histogramas: bench/comparar_histogramas.cpp src/filtros.cpp src/histograma.cpp src/imagen.cpp
	$(CXX) $(CXXFLAGS) -Isrc -o $@ $^

histograma_false_sharing: bench/histograma_false_sharing.cpp src/imagen.cpp
	$(CXX) $(CXXFLAGS) -Isrc -c bench/histograma_false_sharing.cpp -o histograma_false_sharing.o

comparar_false_sharing: bench/comparar_false_sharing.cpp bench/histograma_false_sharing.cpp src/filtros.cpp src/histograma.cpp src/imagen.cpp
	$(CXX) $(CXXFLAGS) -Isrc -o $@ $^

experimento_false_sharing: bench/experimento_false_sharing.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

test_sobel: tests/test_sobel.cpp src/filtros.cpp src/histograma.cpp src/imagen.cpp
	$(CXX) $(CXXFLAGS) -Isrc -o $@ $^

test_histograma: tests/test_histograma.cpp src/histograma.cpp src/imagen.cpp
	$(CXX) $(CXXFLAGS) -Isrc -o $@ $^

test_ecualizacion: tests/test_ecualizacion.cpp src/ecualizacion.cpp src/histograma.cpp src/imagen.cpp
	$(CXX) $(CXXFLAGS) -Isrc -o $@ $^

test_otsu: tests/test_otsu.cpp src/filtros.cpp src/histograma.cpp src/imagen.cpp
	$(CXX) $(CXXFLAGS) -Isrc -o $@ $^

pruebas: all test_sobel test_histograma test_ecualizacion test_otsu
	bash tests/pruebas.sh
	./test_sobel
	./test_histograma
	./test_ecualizacion
	./test_otsu

clean:
	rm -f $(BIN) test_sobel test_histograma test_ecualizacion test_otsu histograma_false_sharing.o experimento_false_sharing

.PHONY: all pruebas clean
