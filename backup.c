#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#include "smart_copy.h"
#include "backup.h"

/*
 * Copia recursivamente un directorio y su contenido.
 * Los elementos especiales (links, dispositivos, etc.) se ignoran.
 * La función falla si cualquier operación crítica no se puede completar.
 */
int copy_directory(const char *src, const char *dest) {
    struct stat st;

    if (stat(src, &st) == -1) {
        perror("Error al obtener stats del directorio de origen");
        return EXIT_FAILURE;
    }

    if (mkdir(dest, st.st_mode) == -1) {
        if (errno != EEXIST) {
            perror("Error al crear el directorio de destino");
            return EXIT_FAILURE;
        }
    } else {
        printf("[OK] Directorio creado: %s\n", dest);
    }

    DIR *dir = opendir(src);
    if (!dir) {
        perror("Error al abrir el directorio de origen");
        return EXIT_FAILURE;
    }

    struct dirent *entry;
    char next_src[1024];
    char next_dest[1024];
    int result = EXIT_SUCCESS;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(next_src, sizeof(next_src), "%s/%s", src, entry->d_name);
        snprintf(next_dest, sizeof(next_dest), "%s/%s", dest, entry->d_name);

        struct stat next_st;
        if (lstat(next_src, &next_st) == -1) {
            perror("Error al obtener los stats de un elemento");
            result = EXIT_FAILURE;
            break;
        }

        if (S_ISDIR(next_st.st_mode)) {
            if (copy_directory(next_src, next_dest) != EXIT_SUCCESS) {
                result = EXIT_FAILURE;
                break;
            }
        } else if (S_ISREG(next_st.st_mode)) {
            int rc = sys_smart_copy(next_src, next_dest);
            if (rc == SMART_COPY_OK) {
                printf("[OK] Archivo respaldado: %s -> %s\n", next_src, next_dest);
            } else {
                fprintf(stderr,
                        "[ERROR] Fallo en sys_smart_copy (%d): %s -> %s. errno=%d (%s)\n",
                        rc,
                        next_src,
                        next_dest,
                        errno,
                        strerror(errno));
                result = EXIT_FAILURE;
                break;
            }
        } else {
            printf("[INFO] Elemento ignorado (especial o link): %s\n", next_src);
        }
    }

    if (closedir(dir) == -1) {
        perror("Error al cerrar el directorio de origen");
        return EXIT_FAILURE;
    }

    return result;
}
