#include "filtros.hpp"
#include <cstddef>
#include <stdexcept>
#include <vector>
#include <memory>
#include <algorithm>

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

namespace {

// Núcleo binomial [1 4 6 4 1] / 16, indexado de -2 a 2 con desplazamiento +2.
constexpr double KERNEL[5] = {1.0 / 16, 4.0 / 16, 6.0 / 16, 4.0 / 16, 1.0 / 16};

// Pasada horizontal: T(x, y) = sum_i k_i * I(x + i, y), en doble precisión.
std::vector<double> pasada_horizontal(const Imagen& e) {
    const std::size_t n = static_cast<std::size_t>(e.ancho) * e.alto * e.canales;
    std::vector<double> T(n);
    for (int y = 0; y < e.alto; ++y) {
        for (int x = 0; x < e.ancho; ++x) {
            for (int c = 0; c < e.canales; ++c) {
                double suma = 0.0;
                for (int i = -2; i <= 2; ++i) {
                    suma += KERNEL[i + 2] * e.en(recortar(x + i, e.ancho), y, c);
                }
                T[(static_cast<std::size_t>(y) * e.ancho + x) * e.canales + c] = suma;
            }
        }
    }
    return T;
}

void pasada_vertical(const double* T, int ancho, int alto, int canales,
                     Imagen& s, int y, int x, int c) {
    double suma = 0.0;
    for (int j = -2; j <= 2; ++j) {
        const int fila = recortar(y + j, alto);
        suma += KERNEL[j + 2] * T[(static_cast<std::size_t>(fila) * ancho + x) * canales + c];
    }
    s.poner(x, y, c, redondear(suma));
}

Imagen crear_salida(const Imagen& e) {
    Imagen s;
    s.ancho = e.ancho;
    s.alto = e.alto;
    s.canales = e.canales;
    s.datos.resize(static_cast<std::size_t>(e.ancho) * e.alto * e.canales);
    return s;
}

}  // namespace

Imagen gaussiano_secuencial(const Imagen& e) {
    const std::vector<double> T = pasada_horizontal(e);
    Imagen s = crear_salida(e);
    for (int y = 0; y < e.alto; ++y) {
        for (int x = 0; x < e.ancho; ++x) {
            for (int c = 0; c < e.canales; ++c) {
                pasada_vertical(T.data(), e.ancho, e.alto, e.canales, s, y, x, c);
            }
        }
    }
    return s;
}

Imagen gaussiano_paralelo(const Imagen& e) {
    const std::size_t n = static_cast<std::size_t>(e.ancho) * e.alto * e.canales;
    // Sin inicializar: la primera escritura la hacen los hilos, así cada
    // hilo toca las páginas de su banda y no un solo hilo todas.
    std::unique_ptr<double[]> T(new double[n]);
    #pragma omp parallel for schedule(static)
    for (int y = 0; y < e.alto; ++y) {
        for (int x = 0; x < e.ancho; ++x) {
            for (int c = 0; c < e.canales; ++c) {
                double suma = 0.0;
                for (int i = -2; i <= 2; ++i) {
                    suma += KERNEL[i + 2] * e.en(recortar(x + i, e.ancho), y, c);
                }
                T[(static_cast<std::size_t>(y) * e.ancho + x) * e.canales + c] = suma;
            }
        }
    }
    Imagen s;
    s.ancho = e.ancho;
    s.alto = e.alto;
    s.canales = e.canales;
    s.datos.resize(n);  // la salida se pone a cero aquí, en un hilo
    #pragma omp parallel for schedule(static)
    for (int y = 0; y < e.alto; ++y) {
        for (int x = 0; x < e.ancho; ++x) {
            for (int c = 0; c < e.canales; ++c) {
                pasada_vertical(T.get(), e.ancho, e.alto, e.canales, s, y, x, c);
            }
        }
    }
    return s;
}

Imagen gaussiano_bloques(const Imagen& e) {
    constexpr int B = 16;  // filas de salida por bloque
    const std::size_t w = static_cast<std::size_t>(e.ancho) * e.canales;  // doubles por fila de T
    Imagen s = crear_salida(e);
    #pragma omp parallel
    {
        // Buffer privado de cada hilo: las filas de T que necesita un bloque,
        // incluidas dos filas arriba y dos abajo.
        std::vector<double> tb(static_cast<std::size_t>(B + 4) * w);
        #pragma omp for schedule(static)
        for (int y0 = 0; y0 < e.alto; y0 += B) {
            const int y1 = std::min(y0 + B, e.alto);
            // t = 0 corresponde a la fila y0 - 2 de T.
            for (int t = 0; t < (y1 - y0) + 4; ++t) {
                const int fila = recortar(y0 - 2 + t, e.alto);
                for (int x = 0; x < e.ancho; ++x) {
                    for (int c = 0; c < e.canales; ++c) {
                        double suma = 0.0;
                        for (int i = -2; i <= 2; ++i) {
                            suma += KERNEL[i + 2] * e.en(recortar(x + i, e.ancho), fila, c);
                        }
                        tb[static_cast<std::size_t>(t) * w +
                           static_cast<std::size_t>(x * e.canales + c)] = suma;
                    }
                }
            }
            // Pasada vertical sobre el buffer del bloque.
            for (int y = y0; y < y1; ++y) {
                for (int x = 0; x < e.ancho; ++x) {
                    for (int c = 0; c < e.canales; ++c) {
                        double suma = 0.0;
                        for (int j = -2; j <= 2; ++j) {
                            const int t = y - y0 + j + 2;
                            suma += KERNEL[j + 2] *
                                    tb[static_cast<std::size_t>(t) * w +
                                       static_cast<std::size_t>(x * e.canales + c)];
                        }
                        s.poner(x, y, c, redondear(suma));
                    }
                }
            }
        }
    }
    return s;
}

Imagen sobel_secuencial(const Imagen&) {
    throw std::runtime_error("sobel: sin implementar");
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
