#ifndef BACKUP_H
#define BACKUP_H

/*
 * Copia recursivamente un directorio y todo su contenido.
 * Retorna EXIT_SUCCESS si toda la operacion termina correctamente.
 */
int copy_directory(const char *src, const char *dest);

/*
 * Copia recursivamente un directorio y solo respalda archivos que coincidan
 * con la extension indicada. Si extension es NULL, se respaldan todos los
 * archivos regulares.
 */
int copy_directory_filtered(const char *src, const char *dest, const char *extension);

/*
 * Retorna 1 si la ruta coincide con la extension indicada o si extension es
 * NULL. Retorna 0 si el archivo debe omitirse por el filtro.
 */
int backup_matches_extension(const char *path, const char *extension);

#endif /* BACKUP_H */
