#include "filtros.hpp"

#include <stdexcept>
#include <cmath>

Imagen gris_secuencial(const Imagen& e) {
    if (e.canales != 3) throw std::runtime_error("gris espera una imagen de tres canales");
    Imagen s;
    s.ancho = e.ancho;
    s.alto = e.alto;
    s.canales = 1;
    s.datos.resize(static_cast<std::size_t>(e.ancho) * e.alto);
    for (int y = 0; y < e.alto; ++y) {
        for (int x = 0; x < e.ancho; ++x) {
            const double v = 0.299 * e.en(x, y, 0) + 0.587 * e.en(x, y, 1) + 0.114 * e.en(x, y, 2);
            s.poner(x, y, 0, redondear(v));
        }
    }
    return s;
}

Imagen gris_paralelo(const Imagen& e) {
    if (e.canales != 3) throw std::runtime_error("gris espera una imagen de tres canales");
    Imagen s;
    s.ancho = e.ancho;
    s.alto = e.alto;
    s.canales = 1;
    s.datos.resize(static_cast<std::size_t>(e.ancho) * e.alto);
    // Cada fila es independiente de las demás: no hay nada que compartir
    // entre hilos ni dependencias entre iteraciones.
    #pragma omp parallel for schedule(static)
    for (int y = 0; y < e.alto; ++y) {
        for (int x = 0; x < e.ancho; ++x) {
            const double v = 0.299 * e.en(x, y, 0) + 0.587 * e.en(x, y, 1) + 0.114 * e.en(x, y, 2);
            s.poner(x, y, 0, redondear(v));
        }
    }
    return s;
}

namespace {

// El promedio de la vecindad de 3x3 de un canal, con las coordenadas
// recortadas al rectángulo de la imagen.
double promedio_vecindad(const Imagen& e, int x, int y, int c) {
    double suma = 0.0;
    for (int j = -1; j <= 1; ++j) {
        const int fila = recortar(y + j, e.alto);
        for (int i = -1; i <= 1; ++i) {
            suma += e.en(recortar(x + i, e.ancho), fila, c);
        }
    }
    return suma / 9.0;
}

}  // namespace

Imagen caja_secuencial(const Imagen& e) {
    Imagen s = e;
    for (int y = 0; y < e.alto; ++y) {
        for (int x = 0; x < e.ancho; ++x) {
            for (int c = 0; c < e.canales; ++c) {
                s.poner(x, y, c, redondear(promedio_vecindad(e, x, y, c)));
            }
        }
    }
    return s;
}

Imagen caja_paralelo(const Imagen& e) {
    Imagen s = e;
    // El ciclo externo recorre filas y el interno columnas, que es el orden
    // en que la imagen está en memoria. Repartir por filas deja a cada hilo
    // con una banda contigua.
    #pragma omp parallel for schedule(static)
    for (int y = 0; y < e.alto; ++y) {
        for (int x = 0; x < e.ancho; ++x) {
            for (int c = 0; c < e.canales; ++c) {
                s.poner(x, y, c, redondear(promedio_vecindad(e, x, y, c)));
            }
        }
    }
    return s;
}

Imagen gaussiano_secuencial(const Imagen&) {
    throw std::runtime_error("gaussiano: sin implementar");
}

Imagen gaussiano_paralelo(const Imagen&) {
    throw std::runtime_error("gaussiano: sin implementar");
}

Imagen sobel_secuencial(const Imagen& e) {
    if (e.canales != 1) {
        throw std::runtime_error("sobel espera una imagen de un canal");
    };

    Imagen s;
    s.ancho = e.ancho;
    s.alto = e.alto;
    s.canales = 1;
    s.datos.resize(static_cast<std::size_t>(e.ancho) * e.alto);
    
    for (int y = 0; y < e.alto; ++y) {
        for (int x = 0; x < e.ancho; ++x) {
            double gx = 0.0;
            double gy = 0.0;

            // Calcular la convolución con el kernel de Sobel
            for (int j = -1; j <= 1; ++j) {
                const int fila = recortar(y + j, e.alto);
                for (int i = -1; i <= 1; ++i) {
                    const int col = recortar(x + i, e.ancho);
                    const double pixel = e.en(col, fila, 0);

                    // Mascara Sobel Horizontal
                    const int peso_x = (i == -1 ? -1 : (i == 1 ? 1 : 0)) * (j == 0 ? 2 : 1);
                    // Mascara Sobel Vertical
                    const int peso_y = (j == -1 ? -1 : (j == 1 ? 1 : 0)) * (i == 0 ? 2 : 1);

                    gx += pixel * peso_x;
                    gy += pixel * peso_y;
                }
            }
            const double magnitude = std::sqrt(gx * gx + gy * gy);

            s.poner(x, y, 0, redondear(magnitude));
        }
    }
    return s;
}

Imagen sobel_paralelo(const Imagen&) {
    throw std::runtime_error("sobel: sin implementar");
}

Imagen otsu_secuencial(const Imagen&) {
    throw std::runtime_error("otsu: sin implementar");
}

Imagen otsu_paralelo(const Imagen&) {
    throw std::runtime_error("otsu: sin implementar");
}
