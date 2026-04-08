Instrucciones de compilacion y ejecucion
========================================

IMPORTANTE:
Este proyecto usa funciones y headers POSIX/Linux como `mkdir(path, mode)`,
`lstat()`, `opendir()`, `readdir()` y `syslog.h`. Por eso debe compilarse en
Linux o en WSL.

Compilacion manual
==================

```bash
gcc -Wall -Wextra -g -o backup_EAFITos backup.c backup_engine.c main.c Benchmark.c
```

Compilacion con Makefile
========================

```bash
make all
```

Uso del programa
================

Mostrar ayuda:

```bash
./backup_EAFITos --help
```

Respaldar un archivo:

```bash
./backup_EAFITos -b archivo.txt copia.txt
```

Respaldar todos los archivos de un directorio:

```bash
./backup_EAFITos -b directorio respaldo --all
```

Respaldar solo una extension:

```bash
./backup_EAFITos -b directorio respaldo_c -e .c
./backup_EAFITos -b directorio respaldo_c --extension c
```

Pruebas automatizadas
=====================

```bash
make test
```

o

```bash
bash run.sh
```

Limpieza
========

```bash
make clean
```
