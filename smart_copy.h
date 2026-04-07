#ifndef SMART_COPY_H
#define SMART_COPY_H

#include <stddef.h>

#define SMART_COPY_BUFFER_SIZE 4096

/*
 * Registro opcional:
 * 0 -> deshabilitado
 * 1 -> habilitado (usa syslog en backup_engine.c)
 */
#ifndef SMART_COPY_ENABLE_SYSLOG
#define SMART_COPY_ENABLE_SYSLOG 1
#endif

/* Codigos de retorno para sys_smart_copy */
#define SMART_COPY_OK 0
#define SMART_COPY_ERR_INVALID_ARG -1
#define SMART_COPY_ERR_STAT -2
#define SMART_COPY_ERR_NOT_REGULAR -3
#define SMART_COPY_ERR_OPEN_SRC -4
#define SMART_COPY_ERR_OPEN_DEST -5
#define SMART_COPY_ERR_READ -6
#define SMART_COPY_ERR_WRITE -7
#define SMART_COPY_ERR_CLOSE -8

/*
 * Copia un archivo regular de src a dest usando E/S de bajo nivel estilo POSIX.
 * Si falla, retorna uno de SMART_COPY_ERR_* y deja errno con el error del sistema.
 */
int sys_smart_copy(const char *src, const char *dest);

#endif /* SMART_COPY_H */
