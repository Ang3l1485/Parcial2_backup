#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include "smart_copy.h"

#define BUFFER_SIZE 4096  // tamaño de página típico

// --------------------------------------------------
// Copia usando librerías estándar (fread / fwrite)
// --------------------------------------------------
int stdio_copy(const char *src, const char *dest) {
    FILE *fsrc = fopen(src, "rb");
    if (!fsrc) {
        perror("Error abriendo archivo fuente (stdio)");
        return -1;
    }

    FILE *fdest = fopen(dest, "wb");
    if (!fdest) {
        perror("Error creando archivo destino (stdio)");
        fclose(fsrc);
        return -1;
    }

    char *buffer = (char *)malloc(BUFFER_SIZE);
    if (!buffer) {
        perror("Error reservando memoria");
        fclose(fsrc);
        fclose(fdest);
        return -1;
    }

    size_t bytes_leidos;

    // lectura en bloques para simular buffering manual
    while ((bytes_leidos = fread(buffer, 1, BUFFER_SIZE, fsrc)) > 0) {
        if (fwrite(buffer, 1, bytes_leidos, fdest) != bytes_leidos) {
            perror("Error escribiendo archivo (stdio)");
            free(buffer);
            fclose(fsrc);
            fclose(fdest);
            return -1;
        }
    }

    if (ferror(fsrc)) {
        perror("Error leyendo archivo (stdio)");
    }

    free(buffer);
    fclose(fsrc);
    fclose(fdest);

    return 0;
}

// --------------------------------------------------
// Función genérica para medir tiempo de ejecución
// --------------------------------------------------
double medir_tiempo(int (*func)(const char*, const char*),
                    const char *src,
                    const char *dest) {

    clock_t inicio = clock();

    if (func(src, dest) != 0) {
        fprintf(stderr, "Error ejecutando la función de copia\n");
        return -1.0;
    }

    clock_t fin = clock();

    return (double)(fin - inicio) / CLOCKS_PER_SEC;
}

// --------------------------------------------------
// Ejecuta comparación entre stdio y sys_smart_copy
// --------------------------------------------------
void ejecutar_benchmark(const char *src,
                        const char *dest_stdio,
                        const char *dest_sys) {

    double tiempo_stdio, tiempo_sys;

    printf("\n=== Benchmark de copia de archivos ===\n");
    printf("Archivo fuente: %s\n", src);

    // medir copia con stdio
    tiempo_stdio = medir_tiempo(stdio_copy, src, dest_stdio);

    // medir copia con syscall simulada
    tiempo_sys = medir_tiempo(sys_smart_copy, src, dest_sys);

    printf("\n--- Resultados ---\n");

    if (tiempo_stdio >= 0)
        printf("STDIO (fread/fwrite): %.6f segundos\n", tiempo_stdio);

    if (tiempo_sys >= 0)
        printf("SMART COPY (syscall): %.6f segundos\n", tiempo_sys);

    if (tiempo_stdio > 0 && tiempo_sys > 0) {
        printf("Diferencia (stdio - syscall): %.6f segundos\n",
               tiempo_stdio - tiempo_sys);
    }

    printf("\n");
}