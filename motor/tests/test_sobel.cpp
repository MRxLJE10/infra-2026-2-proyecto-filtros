#include "filtros.hpp"

#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
    int fallos = 0;

    void comprobar(const char* nombre, const Imagen& obtenida, const std::vector<unsigned char>& esperada){
        if (obtenida.datos == esperada){
            std::cout << " OK " << nombre << '\n';
        } else {
            std::cout << " FAIL " << nombre << '\n';
            fallos++;

            std::cout << "  Esperada: ";
            for (unsigned char pixel : esperada){
                std::cout << static_cast<int>(pixel) << " ";
            }
            std::cout << "\n  Obtenido: ";
            for (unsigned char pixel : obtenida.datos){
                std::cout << static_cast<int>(pixel) << " ";
            }
            std::cout << "\n";
        }
    }

    Imagen crear_imagen(int ancho, int alto, const std::vector<unsigned char>& datos){
        Imagen imagen;
        imagen.ancho = ancho;
        imagen.alto = alto;
        imagen.canales = 1;
        imagen.datos = datos;
        return imagen;
    }
}

int main() {
    std::cout << "Pruebas de correctitud de Sobel:\n";

    // Prueba 1: Imagen constante no debe tener gradiente
    {
        Imagen entrada = crear_imagen(3, 3, {
            50, 50, 50,
            50, 50, 50,
            50, 50, 50
        });

        Imagen salida = sobel_secuencial(entrada);

        comprobar("Iamgen constante produce gradiente cerpo", salida, {
            0, 0, 0,
            0, 0, 0,
            0, 0, 0
        });
    }

    // Prueba 2: Deteccion de un gradiente horizontal
    {
        Imagen entrada = crear_imagen(3, 3, {
            0, 10, 20,
            0, 10, 20,
            0, 10, 20
        });

        Imagen salida = sobel_secuencial(entrada);

        comprobar("Gradiente horizontal", salida, {
            40, 80, 40,
            40, 80, 40,
            40, 80, 40
        });
    }

    // Prueba 3: Deteccion de un gradiente vertical
    {
        Imagen entrada = crear_imagen(3, 3, {
            0, 0, 0,
            10, 10, 10,
            20, 20, 20
        });

        Imagen salida = sobel_secuencial(entrada);

        comprobar("Gradiente vertical", salida, {
            40, 40, 40,
            80, 80, 80,
            40, 40, 40
        });
    }

    // Prueba 4: Sobel debe rechazar imagen RGB
    {
        Imagen entrada;
        entrada.ancho = 2;
        entrada.alto = 2;
        entrada.canales = 3;
        entrada.datos.resize(12, 100);

        try {
            (void)sobel_secuencial(entrada);
            std::cout << " FAIL Sobel acepta imagen RGB\n";
            fallos++;
        } catch (const std::runtime_error&) {
            std::cout << " OK Sobel rechaza imagen RGB\n";
        }
    }

    std::cout << '\n';
    if (fallos == 0) {
        std::cout << "Todas las pruebas pasaron correctamente.\n";
        return 0;
    }

    std::cout << fallos << " pruebas fallaron.\n";
    return 1;
}