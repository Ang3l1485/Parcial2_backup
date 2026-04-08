# Smart Backup Kernel-Space Utility

Proyecto de parcial para Sistemas Operativos. El objetivo es implementar una copia de archivos con estilo syscall (`sys_smart_copy`) y compararla contra una version basada en `stdio`, manteniendo una interfaz de usuario clara para respaldar archivos y directorios.

## Estado actual del proyecto

El proyecto ya incluye:

- `sys_smart_copy()` con buffer de `4096` bytes y manejo de errores por codigo de retorno.
- Respaldo de archivos regulares y directorios de forma recursiva.
- Comparacion basica entre copia tipo syscall y copia con `stdio`.
- Opcion de respaldar todos los archivos o solo archivos con una extension especifica.
- `Makefile` y `run.sh` para compilar y ejecutar pruebas funcionales.

## Cambios importantes agregados

Se agrego soporte para filtrar backups por extension:

- Respaldo completo de todos los archivos: `--all`
- Respaldo solo de una extension: `-e .c` o `--extension c`

Esto aplica especialmente al respaldo recursivo de directorios. Los archivos que no coinciden con el filtro se omiten y se reportan en consola.

## Estructura del proyecto

- `smart_copy.h`: firma de `sys_smart_copy`, codigos de retorno y configuracion del buffer.
- `backup_engine.c`: implementacion de la copia de bajo nivel con `open`, `read`, `write` y `close`.
- `backup.h`: interfaz del respaldo recursivo y del filtro por extension.
- `backup.c`: recorrido recursivo del arbol de directorios y aplicacion del filtro.
- `main.c`: CLI principal, validacion de argumentos y ejecucion del benchmark para copia de archivos.
- `Benchmark.c` y `Benchmark.h`: comparativa entre `stdio` y `sys_smart_copy`.
- `Makefile`: compilacion y pruebas automatizadas.
- `run.sh`: flujo guiado de compilacion y validacion.

## Requisitos del entorno

Este proyecto esta pensado para Linux o WSL. Usa llamadas y headers POSIX como:

- `mkdir(path, mode)`
- `lstat()`
- `opendir()` / `readdir()`
- `syslog.h`

En Windows nativo con MinGW puede fallar la compilacion por diferencias de plataforma.

## Compilacion

Compilacion manual:

```bash
gcc -Wall -Wextra -g -o backup_EAFITos backup.c backup_engine.c main.c Benchmark.c
```

Compilacion con `make`:

```bash
make all
```

## Uso

Mostrar ayuda:

```bash
./backup_EAFITos --help
```

Respaldar un archivo:

```bash
./backup_EAFITos -b archivo.txt copia.txt
```

Respaldar todo el contenido de un directorio:

```bash
./backup_EAFITos -b proyecto respaldo --all
```

Respaldar solo archivos `.c` dentro de un directorio:

```bash
./backup_EAFITos -b proyecto respaldo_c -e .c
```

Tambien se acepta la extension sin punto:

```bash
./backup_EAFITos -b proyecto respaldo_c --extension c
```

## Pruebas

Con `make`:

```bash
make test
```

Con script:

```bash
bash run.sh
```

Las pruebas cubren:

- Copia de archivo simple.
- Copia recursiva de directorio completo.
- Verificacion de contenido.
- Filtro por extension para copiar solo archivos `.c`.

## Benchmark

Cuando se respalda un archivo individual, el programa ejecuta una comparativa entre:

- `stdio_copy()` usando `fread` / `fwrite`
- `sys_smart_copy()` usando llamadas de bajo nivel

Los resultados se guardan en consola y se generan dos archivos temporales:

- `bench_stdio.dat`
- `bench_sys.dat`

## Revision frente a la rubrica

Segun el PDF de la rubrica, el proyecto pide principalmente:

1. Una funcion `sys_smart_copy` con manejo correcto de errores y buffer de 4KB.
2. Comparativa entre enfoque syscall y `stdio`.
3. Manejo de errores usando `errno`.
4. Codigo limpio, modular y bien organizado.
5. Un `reporte.pdf` con el analisis tecnico.

### Lo que ya queda mejor cubierto en el codigo

- `sys_smart_copy` usa buffer de 4096 bytes.
- El proyecto esta separado en archivos modulares.
- El respaldo recursivo ahora soporta filtro por extension y modo completo.
- `Makefile` y `run.sh` compilan tambien `Benchmark.c`, que antes no estaba integrado.

### Lo que sigue siendo una entrega aparte

- El archivo `reporte.pdf` todavia debe prepararse como documento de entrega.
- Para apuntarle al maximo nivel de la rubrica en rendimiento, conviene ejecutar y documentar pruebas con archivos de al menos `1KB`, `1MB` y `1GB`, y luego incluir esa tabla comparativa en el reporte.

## Archivos esperados para la entrega

De acuerdo con la rubrica, la entrega deberia incluir como minimo:

- `smart_copy.h`
- `backup_engine.c`
- `main.c`
- `reporte.pdf`

## Limpieza

```bash
make clean
```

Esto elimina el ejecutable, objetos, archivos temporales del benchmark y carpetas de prueba.
