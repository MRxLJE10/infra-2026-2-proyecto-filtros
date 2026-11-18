#include "filtros.hpp"

#include <stdexcept>
#include <iostream>
#include <cassert>

int main(){
    // Prueba 1: Intensidades diferentes
    {
        Imagen entrada;
        entrada.ancho = 4;
        entrada.alto = 1;
        entrada.canales = 1;
        entrada.datos = {10, 20, 30 , 40};

        Imagen salida = ecualizacion_secuencial(entrada);

        assert(salida.datos == std::vector<unsigned char>({0, 85, 170, 255}));
    }

    // Prueba 2: Intensidades repetidas
    {
        Imagen entrada;
        entrada.ancho = 4;
        entrada.alto = 1;
        entrada.canales = 1;
        entrada.datos = {10, 10, 20, 30};

        Imagen salida = ecualizacion_secuencial(entrada);

        assert(salida.datos == std::vector<unsigned char>({0, 0, 128, 255}));
    }

    // Prueba 3: Imagen de intensidad constante
    {
        Imagen entrada;
        entrada.ancho = 3;
        entrada.alto = 1;
        entrada.canales = 1;
        entrada.datos = {42, 42, 42};

        Imagen salida = ecualizacion_secuencial(entrada);

        assert(salida.datos == entrada.datos);
    }

    // Prueba 4: Rechazar imagen RGB
    {
        Imagen entrada;
        entrada.ancho = 1;
        entrada.alto = 1;
        entrada.canales = 3;
        entrada.datos = {10, 20, 30};

        bool excepcion = false;

        try {
            ecualizacion_secuencial(entrada);
        } catch (const std::runtime_error&) {
            excepcion = true;
        }
        assert(excepcion);
    }

    // Prueba 5: Rechazar imagen vacía
    {
        Imagen entrada;
        entrada.ancho = 0;
        entrada.alto = 0;
        entrada.canales = 1;

        bool excepcion = false;

        try {
            ecualizacion_secuencial(entrada);
        } catch (const std::runtime_error&) {
            excepcion = true;
        }
        assert(excepcion);
    }

    std::cout << "Todas las pruebas pasaron exitosamente\n";
    return 0;
}