Resumen de cambios recientes
============================

1. Filtro por extension en backups
----------------------------------

Se agrego soporte para dos modos de respaldo:

- `--all`: copia todos los archivos.
- `-e .ext` o `--extension ext`: copia solo archivos con esa extension.

El filtro aplica al recorrido recursivo de directorios y tambien a archivos
individuales.

2. Integracion correcta del benchmark
-------------------------------------

Se corrigio la compilacion para incluir `Benchmark.c`, que ya existia en el
proyecto pero no estaba siendo enlazado desde `Makefile` ni desde `run.sh`.

3. Pruebas automatizadas actualizadas
-------------------------------------

Las pruebas ahora validan:

- Copia de archivo simple.
- Copia recursiva completa.
- Integridad del contenido copiado.
- Filtro por extension para copiar solo archivos `.c`.

5. Scripts coherentes
---------------------

- `run.sh` compila y prueba el proyecto completo.
- `run` ahora delega directamente a `run.sh` para evitar divergencias.
