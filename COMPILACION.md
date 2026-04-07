Instrucciones de compilación y ejecución
=========================================

IMPORTANTE: Este proyecto utiliza syscalls de POSIX/Linux:
  - mkdir(path, mode) con DOS argumentos
  - lstat() para detectar tipos de archivos
  - syslog.h para logging del sistema
  - Llamadas específicas de Linux (opendir, readdir, etc.)

Por lo tanto, DEBE compilarse en un sistema Linux o en WSL (Windows Subsystem for Linux).

OPCIÓN 1: En Linux nativo o WSL
================================

1. Abre una terminal en WSL o un sistema Linux

2. Ve a la carpeta del proyecto:
   $ cd /ruta/a/Parcial2_backup

3. Compila manualmente (opción recomendada):
   $ gcc -Wall -Wextra -g -o backup_EAFITos backup.c backup_engine.c main.c

   O usa el Makefile:
   $ make all

4. Prueba con ayuda:
   $ ./backup_EAFITos --help

5. Realiza pruebas funcionales:
   $ make test
   
   O ejecuta el script de ejecución:
   $ bash run.sh

OPCIÓN 2: Usar WSL desde Windows
==================================

Si tienes Windows, puedes usar WSL (Windows Subsystem for Linux):

1. Instala WSL si aún no lo tienes:
   - Abre PowerShell como administrador
   - wsl --install
   - Reinicia tu computadora

2. Abre WSL:
   - Escribe "wsl" en PowerShell o busca "Ubuntu" en el menú Inicio

3. Clona y accede al proyecto:
   $ git clone https://github.com/Ang3l1485/Parcial2_backup
   $ cd Parcial2_backup

4. Compila como se indica en OPCIÓN 1

Modo de uso del programa compilado
===================================

Una vez compilado, tienes el ejecutable 'backup_EAFITos':

   ./backup_EAFITos -h
   ./backup_EAFITos --help
   
   ./backup_EAFITos -b archivo.txt copia.txt
   ./backup_EAFITos -b directorio/ respaldo_directorio/

Verificación rápida
====================

Después de compilar:

1. Ver la ayuda:
   $ ./backup_EAFITos --help

2. Copiar un archivo:
   $ echo "contenido" > archivo_prueba.txt
   $ ./backup_EAFITos -b archivo_prueba.txt archivo_copia.txt
   $ cmp archivo_prueba.txt archivo_copia.txt  # Verifica que sean idénticos

3. Copiar un directorio:
   $ mkdir -p directorio_test/subdir
   $ echo "archivo 1" > directorio_test/archivo1.txt
   $ echo "archivo en subdir" > directorio_test/subdir/archivo2.txt
   $ ./backup_EAFITos -b directorio_test backup_test
   $ ls -R backup_test  # Verifica la estructura

Estructura del proyecto
=======================

smart_copy.h
  - Define sys_smart_copy(src, dest) y códigos de retorno
  - Define SMART_COPY_BUFFER_SIZE = 4096

backup_engine.c
  - Implementación de bajo nivel de sys_smart_copy
  - Usa open, read, write, close, stat
  - Incluye logging con syslog

backup.h
  - Header que declara copy_directory()

backup.c
  - Función copy_directory() para copias recursivas
  - Usa mkdir, opendir, readdir, lstat
  - Llama sys_smart_copy para archivos regulares

main.c
  - Función main() con parseo de CLI
  - Carga help (-h, --help)
  - Opción -b, --backup
  - Detecta si es archivo o directorio y usa la función correcta

Makefile
  - Compila todos los .c: backup.c, backup_engine.c, main.c
  - Target: backup_EAFITos
  - Targets utilities: all, clean, test

run.sh
  - Script bash de compilación y pruebas
  - Verifica que archivos se copien correctamente
  - Verifica contenido con cmp
  - Falla si alguna prueba no pasa

