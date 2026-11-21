#!/usr/bin/env bash
# Corre el arnés para 1, 2, 4 y 8 hilos sobre la imagen de medición y deja
# la tabla en mediciones/motor.csv.
#
#   bash bench/medir.sh imagen.ppm gris caja
set -euo pipefail

if [[ $# -lt 2 ]]; then
    echo "uso: bash bench/medir.sh <imagen.ppm> <filtro> [filtro...]" >&2
    exit 2
fi

imagen="$1"
shift
salida="../mediciones/motor.csv"
repeticiones=5

echo "filtro,hilos,repeticion,milisegundos" > "$salida"
for filtro in "$@"; do
    for hilos in 1 2 4 8; do
        OMP_NUM_THREADS="$hilos" ./arnes "$filtro" "$imagen" "$repeticiones" >> "$salida"
    done
done

echo "escrito $salida con $(( $(wc -l < "$salida") - 1 )) filas"
