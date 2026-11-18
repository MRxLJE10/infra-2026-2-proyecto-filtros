#include "filtros.hpp"

#include <stdexcept>
#include <iostream>

int main() {
    Imagen img;
    img.ancho = 3;
    img.alto = 2;
    img.canales = 1;
    img.datos = {
        10, 20, 10,
        30, 20, 10
    };
/*
    const Histograma h = histograma_secuencial(img);

    if (h[10] != 3 || h[20] != 2 || h[30] != 1) {
        std::cerr << "Error: Frecuencias incorrectas\n" << std::endl;
        return 1;
    }

    if (h[0] != 0 || h[255] != 0) {
        std::cerr << "Error: Contadores inesperados\n" << std::endl;
        return 1;
    }

    std::cout << "Todas las pruebas de Histograma pasaron\n";
    return 0;
}


    const Histograma secuencial = histograma_secuencial(img);

    if (secuencial[10] != 3 || secuencial[20] != 2 || secuencial[30] != 1) {
        std::cerr << "Error: Frecuencias secuenciales incorrectas\n";
        return 1;
    }

    const Histograma paralelo = histograma_paralelo_compartido(img, 4);

    if (paralelo != secuencial) {
        std::cerr << "Error: Frecuencias paralelas no coinciden\n";
        return 1;
    }

    std::cout << "Todas las pruebas de Histograma pasaron\n";
    return 0;
}
*/

const Histograma secuencial = histograma_secuencial(img);

const Histograma compartido = histograma_paralelo_compartido(img, 4);

const Histograma privado = histograma_paralelo_privado(img, 4);

if (secuencial[10] != 3 || secuencial[20] != 2 || secuencial[30] != 1) {
    std::cerr << "Error: Frecuencias incorrectas\n";
    return 1;
}

if (compartido != secuencial) {
    std::cerr << "Error: Frecuencias compartidas no coinciden\n";
    return 1;
}

if (privado != secuencial) {
    std::cerr << "Error: Frecuencias privadas no coinciden\n";
    return 1;
}

std::cout << "Todas las pruebas de Histograma pasaron\n";
return 0;
}