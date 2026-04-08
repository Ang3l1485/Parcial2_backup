#!/bin/bash

set -e

cat << 'HEADER'
==========================================
     SMART BACKUP - COMPILACION Y PRUEBA
==========================================

DESCRIPCION:
  Sistema de respaldo recursivo que usa syscalls de bajo nivel
  para copiar archivos y directorios.

MODO DE USO:
  - Compilar:              ./run.sh
  - Con ayuda:             ./backup_EAFITos --help
  - Copiar archivo:        ./backup_EAFITos -b archivo.txt copia.txt
  - Copiar todo:           ./backup_EAFITos -b origen/ destino/ --all
  - Filtrar extension:     ./backup_EAFITos -b origen/ destino/ -e .c

REQUISITOS:
  - gcc con flags: -Wall -Wextra -g
  - Linux/POSIX con syscalls: open, read, write, stat, mkdir, opendir

==========================================
HEADER

echo "[1/4] Compilando el programa..."
gcc -Wall -Wextra -g -o backup_EAFITos backup.c backup_engine.c main.c Benchmark.c
echo "  -> Compilacion exitosa"

echo ""
echo "[2/4] Mostrando ayuda del CLI..."
./backup_EAFITos --help

echo ""
echo "[3/4] Preparando entorno de prueba..."
rm -rf test_resources backup_dir backup_dir_c
rm -f backup_archivo1.txt bench_stdio.dat bench_sys.dat
mkdir -p test_resources/subdir
echo "Contenido del archivo 1" > test_resources/archivo1.txt
echo "Contenido del archivo 2" > test_resources/archivo2.txt
echo "int main(void) { return 0; }" > test_resources/main.c
echo "Archivo en subdirectorio" > test_resources/subdir/archivo3.txt
echo "#include <stdio.h>" > test_resources/subdir/archivo4.c

echo "  -> Estructura de prueba creada"
echo "     test_resources/"
echo "       - archivo1.txt"
echo "       - archivo2.txt"
echo "       - main.c"
echo "       - subdir/archivo3.txt"
echo "       - subdir/archivo4.c"

echo ""
echo "[4/4] Ejecutando pruebas funcionales..."

echo ""
echo "  TEST 1: Copia de archivo simple"
./backup_EAFITos -b test_resources/archivo1.txt backup_archivo1.txt
if cmp -s test_resources/archivo1.txt backup_archivo1.txt; then
    echo "  -> EXITO: archivo copiado y verificado"
else
    echo "  -> FALLO: contenido de archivo no coincide"
    exit 1
fi

echo ""
echo "  TEST 2: Copia de directorio completa"
./backup_EAFITos -b test_resources backup_dir --all
if [ -f backup_dir/archivo1.txt ] && [ -f backup_dir/archivo2.txt ] && \
   [ -f backup_dir/main.c ] && [ -f backup_dir/subdir/archivo3.txt ] && \
   [ -f backup_dir/subdir/archivo4.c ]; then
    echo "  -> EXITO: estructura de directorio copiada"
else
    echo "  -> FALLO: falta algun archivo en la copia"
    exit 1
fi

echo ""
echo "  TEST 3: Verificar contenido de archivos en copia completa"
if cmp -s test_resources/archivo1.txt backup_dir/archivo1.txt && \
   cmp -s test_resources/subdir/archivo3.txt backup_dir/subdir/archivo3.txt; then
    echo "  -> EXITO: contenido de archivos identico"
else
    echo "  -> FALLO: contenido de archivos no coincide"
    exit 1
fi

echo ""
echo "  TEST 4: Copia de directorio filtrando solo .c"
./backup_EAFITos -b test_resources backup_dir_c -e .c
if [ -f backup_dir_c/main.c ] && [ -f backup_dir_c/subdir/archivo4.c ] && \
   [ ! -f backup_dir_c/archivo1.txt ] && [ ! -f backup_dir_c/subdir/archivo3.txt ]; then
    echo "  -> EXITO: filtro por extension funcionando"
else
    echo "  -> FALLO: el filtro por extension no produjo el resultado esperado"
    exit 1
fi

echo ""
echo "=========================================="
echo "TODAS LAS PRUEBAS FUERON EXITOSAS"
echo "=========================================="
echo ""
echo "Archivos generados:"
echo "  - backup_EAFITos (ejecutable)"
echo "  - backup_archivo1.txt (copia de archivo)"
echo "  - backup_dir/ (copia completa)"
echo "  - backup_dir_c/ (copia solo .c)"
echo "  - bench_stdio.dat y bench_sys.dat (benchmark)"
echo ""
echo "Para limpiar todo: make clean"
echo ""
