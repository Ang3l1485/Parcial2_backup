#ifndef BACKUP_H
#define BACKUP_H

/*
 * Copia recursivamente un directorio y su contenido.
 * Retorna EXIT_SUCCESS si toda la operación termina correctamente.
 * Retorna EXIT_FAILURE si falla cualquier paso (stat, mkdir, opendir,
 * lstat, copy recursivo, sys_smart_copy o closedir).
 */
int copy_directory(const char *src, const char *dest);

#endif /* BACKUP_H */
