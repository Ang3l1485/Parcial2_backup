#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "Benchmark.h"
#include "backup.h"
#include "smart_copy.h"

#define BACKUP_EXTENSION_MAX 64

/*
 * Muestra el mensaje de ayuda del programa.
 */
static void print_help(const char *prog_name) {
    printf("==========================================\n");
    printf("      SISTEMA DE BACKUP C - SysCalls      \n");
    printf("==========================================\n");
    printf("Uso: %s [OPCION] [ORIGEN] [DESTINO] [FILTRO]\n", prog_name);
    printf("\nPrueba de concepto de un sistema de copias de seguridad usando las\n");
    printf("llamadas al sistema POSIX/Linux (open, read, write, close, mkdir, etc.).\n\n");
    printf("Opciones:\n");
    printf("  -h, --help             Muestra esta ayuda.\n");
    printf("  -b, --backup           Realiza respaldo de un archivo o directorio.\n");
    printf("  -a, --all              Respalda todos los archivos (modo por defecto).\n");
    printf("  -e, --extension EXT    Solo respalda archivos con la extension EXT.\n");
    printf("\nEjemplos:\n");
    printf("  %s -b archivo.txt backup_archivo.txt\n", prog_name);
    printf("  %s -b proyecto respaldo --all\n", prog_name);
    printf("  %s -b proyecto respaldo -e .c\n", prog_name);
    printf("  %s -b proyecto respaldo --extension txt\n\n", prog_name);
}

static int normalize_extension(const char *input, char *buffer, size_t buffer_size) {
    size_t input_len;
    size_t required_len;

    if (input == NULL || input[0] == '\0') {
        return -1;
    }

    input_len = strlen(input);
    required_len = input_len + 1;
    if (input[0] != '.') {
        required_len++;
    }

    if (required_len > buffer_size) {
        return -1;
    }

    if (input[0] == '.') {
        snprintf(buffer, buffer_size, "%s", input);
    } else {
        snprintf(buffer, buffer_size, ".%s", input);
    }

    return 0;
}

static int parse_backup_mode(int argc,
                             char *argv[],
                             const char **src,
                             const char **dest,
                             const char **extension,
                             char *extension_buffer,
                             size_t extension_buffer_size) {
    int i;
    int seen_all = 0;
    int seen_extension = 0;

    if (argc < 4) {
        fprintf(stderr, "Error: faltan argumentos. Forma correcta: %s -b origen destino [--all | -e .ext]\n",
                argv[0]);
        return -1;
    }

    *src = argv[2];
    *dest = argv[3];
    *extension = NULL;

    for (i = 4; i < argc; ++i) {
        if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--all") == 0) {
            if (seen_all || seen_extension) {
                fprintf(stderr, "Error: usa solo un modo de filtro por respaldo.\n");
                return -1;
            }

            seen_all = 1;
        } else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--extension") == 0) {
            if (seen_all || seen_extension) {
                fprintf(stderr, "Error: usa solo un modo de filtro por respaldo.\n");
                return -1;
            }

            if (i + 1 >= argc) {
                fprintf(stderr, "Error: falta la extension despues de %s.\n", argv[i]);
                return -1;
            }

            if (normalize_extension(argv[i + 1], extension_buffer, extension_buffer_size) != 0) {
                fprintf(stderr, "Error: la extension indicada no es valida o es demasiado larga.\n");
                return -1;
            }

            *extension = extension_buffer;
            seen_extension = 1;
            ++i;
        } else {
            fprintf(stderr, "Error: opcion no reconocida '%s'.\n", argv[i]);
            return -1;
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    char extension_buffer[BACKUP_EXTENSION_MAX];
    const char *src;
    const char *dest;
    const char *extension;
    struct stat st;

    if (argc < 2) {
        print_help(argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        print_help(argv[0]);
        return EXIT_SUCCESS;
    }

    if (strcmp(argv[1], "-b") == 0 || strcmp(argv[1], "--backup") == 0) {
        if (parse_backup_mode(argc,
                              argv,
                              &src,
                              &dest,
                              &extension,
                              extension_buffer,
                              sizeof(extension_buffer)) != 0) {
            return EXIT_FAILURE;
        }

        if (stat(src, &st) == -1) {
            perror("Error comprobando el archivo/directorio de origen");
            return EXIT_FAILURE;
        }

        if (S_ISDIR(st.st_mode)) {
            printf("--- Iniciando respaldo del directorio '%s' en '%s' ---\n", src, dest);
            if (extension != NULL) {
                printf("--- Filtro activo: solo archivos con extension '%s' ---\n", extension);
            } else {
                printf("--- Filtro activo: todos los archivos ---\n");
            }

            if (copy_directory_filtered(src, dest, extension) != EXIT_SUCCESS) {
                fprintf(stderr, "Error: fallo el respaldo recursivo del directorio.\n");
                return EXIT_FAILURE;
            }

            printf("--- Respaldo completado ---\n");
        } else if (S_ISREG(st.st_mode)) {
            int rc;

            if (!backup_matches_extension(src, extension)) {
                printf("[INFO] Archivo omitido por filtro de extension: %s\n", src);
                return EXIT_SUCCESS;
            }

            printf("--- Iniciando respaldo del archivo '%s' en '%s' ---\n", src, dest);
            rc = sys_smart_copy(src, dest);
            if (rc == SMART_COPY_OK) {
                printf("[OK] Archivo respaldado: %s -> %s\n", src, dest);
                printf("--- Respaldo completado ---\n");

                printf("\n--- Ejecutando benchmark ---\n");
                ejecutar_benchmark(src, "bench_stdio.dat", "bench_sys.dat");
            } else {
                fprintf(stderr, "Error en sys_smart_copy (%d). errno=%d (%s)\n",
                        rc,
                        errno,
                        strerror(errno));
                return EXIT_FAILURE;
            }
        } else {
            fprintf(stderr, "Error: el origen no es valido. Debe ser archivo regular o directorio.\n");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    fprintf(stderr, "Error: opcion no reconocida '%s'.\n", argv[1]);
    print_help(argv[0]);
    return EXIT_FAILURE;
}
