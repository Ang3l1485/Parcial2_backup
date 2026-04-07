#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include "smart_copy.h"
#include "backup.h"

/*
 * Muestra el mensaje de ayuda del programa.
 */
static void print_help(const char *prog_name) {
    printf("==========================================\n");
    printf("      SISTEMA DE BACKUP C - SysCalls      \n");
    printf("==========================================\n");
    printf("Uso: %s [OPCION] [ORIGEN] [DESTINO]\n", prog_name);
    printf("\nPrueba de concepto de un sistema de copias de seguridad usando las\n");
    printf("llamadas al sistema de POSIX/Linux (open, read, write, close, mkdir, etc.).\n\n");
    printf("Opciones:\n");
    printf("  -h, --help    Muestra esta ayuda.\n");
    printf("  -b, --backup  Realizar respaldo de un archivo o directorio recursivamente.\n");
    printf("\nEjemplos:\n");
    printf("  %s -b archivo.txt backup_archivo.txt\n", prog_name);
    printf("  %s -b /home/user/documentos /tmp/backup_documentos\n\n", prog_name);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help(argv[0]);
        return EXIT_FAILURE;
    }

    /* Opción -h o --help */
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        print_help(argv[0]);
        return EXIT_SUCCESS;
    }

    /* Opción -b o --backup */
    if (strcmp(argv[1], "-b") == 0 || strcmp(argv[1], "--backup") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Error: Faltan argumentos. Forma correcta: %s -b origen destino\n", argv[0]);
            return EXIT_FAILURE;
        }

        const char *src = argv[2];
        const char *dest = argv[3];

        struct stat st;
        
        /* Verificar que el origen existe antes de intentar copiar */
        if (stat(src, &st) == -1) {
            perror("Error comprobando el archivo/directorio de origen");
            return EXIT_FAILURE;
        }

        if (S_ISDIR(st.st_mode)) {
            /* Es un directorio: usar copy_directory */
            printf("--- Iniciando respaldo del directorio '%s' en '%s' ---\n", src, dest);
            if (copy_directory(src, dest) != EXIT_SUCCESS) {
                fprintf(stderr, "Error: Fallo el respaldo recursivo del directorio.\n");
                return EXIT_FAILURE;
            }
            printf("--- Respaldo completado ---\n");
        } else if (S_ISREG(st.st_mode)) {
            /* Es un archivo regular: usar sys_smart_copy */
            printf("--- Iniciando respaldo del archivo '%s' en '%s' ---\n", src, dest);

            int rc = sys_smart_copy(src, dest);
            if (rc == SMART_COPY_OK) {
                printf("[OK] Archivo respaldado: %s -> %s\n", src, dest);
                printf("--- Respaldo completado ---\n");
            } else {
                fprintf(stderr, "Error en sys_smart_copy (%d). errno=%d (%s)\n", rc, errno, strerror(errno));
                return EXIT_FAILURE;
            }
        } else {
            fprintf(stderr, "Error: El origen no es valido. Debe ser archivo regular o directorio.\n");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    /* Opción no reconocida */
    fprintf(stderr, "Error: Opcion no reconocida '%s'.\n", argv[1]);
    print_help(argv[0]);
    return EXIT_FAILURE;
}
