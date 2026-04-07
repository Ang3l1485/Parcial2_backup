CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = backup_EAFITos
SRC = backup.c backup_engine.c main.c
OBJ = $(SRC:.c=.o)

# Regla por defecto (compila el programa principal)
all: $(TARGET)

# Cómo construir el ejecutable desde archivos objeto
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Regla genérica para compilar .c a .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regla para limpiar los archivos generados y los de prueba
clean:
	rm -f $(TARGET) $(OBJ)
	rm -rf src_test dest_test test_file.txt test_file_backup.txt

# Regla para ejecutar pruebas automáticas
test: $(TARGET)
	@echo "\n--- [1] PREPARANDO ENTORNO DE PRUEBA ---"
	mkdir -p src_test
	echo "Hola, este es un archivo de prueba 1" > src_test/archivo1.txt
	echo "Esto es otra prueba" > src_test/archivo2.txt
	mkdir -p src_test/subcarpeta
	echo "Archivo en subcarpeta" > src_test/subcarpeta/archivo3.txt
	echo "Archivo simple" > test_file.txt

	@echo "\n--- [2] PROBANDO BACKUP DE ARCHIVO SIMPLE ---"
	./$(TARGET) -b test_file.txt test_file_backup.txt
	@test -f test_file_backup.txt && echo "[EXITO] Archivo copiado" || (echo "[FALLO] Archivo no existe" && exit 1)
	@cmp test_file.txt test_file_backup.txt && echo "[EXITO] Contenido identico" || (echo "[FALLO] Contenido diferente" && exit 1)

	@echo "\n--- [3] PROBANDO BACKUP DE DIRECTORIO ---"
	./$(TARGET) -b src_test dest_test

	@echo "\n--- [4] VERIFICANDO ESTRUCTURA DE DIRECTORIO ---"
	@test -f dest_test/archivo1.txt && echo "[EXITO] archivo1.txt copiado" || (echo "[FALLO] archivo1.txt no existe" && exit 1)
	@test -f dest_test/archivo2.txt && echo "[EXITO] archivo2.txt copiado" || (echo "[FALLO] archivo2.txt no existe" && exit 1)
	@test -f dest_test/subcarpeta/archivo3.txt && echo "[EXITO] subcarpeta/archivo3.txt copiado" || (echo "[FALLO] subcarpeta/archivo3.txt no existe" && exit 1)

	@echo "\n--- [5] VERIFICANDO CONTENIDO DE ARCHIVOS ---"
	@cmp src_test/archivo1.txt dest_test/archivo1.txt && echo "[EXITO] archivo1.txt contenido identico" || (echo "[FALLO] archivo1.txt contenido diferente" && exit 1)
	@cmp src_test/subcarpeta/archivo3.txt dest_test/subcarpeta/archivo3.txt && echo "[EXITO] subcarpeta/archivo3.txt contenido identico" || (echo "[FALLO] subcarpeta/archivo3.txt contenido diferente" && exit 1)

	@echo "\n--- [6] PRUEBAS COMPLETADAS EXITOSAMENTE ---"

.PHONY: all clean test
