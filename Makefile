CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = backup_EAFITos
SRC = backup.c backup_engine.c main.c Benchmark.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)
	rm -rf src_test dest_test dest_test_c
	rm -f test_file.txt test_file_backup.txt
	rm -f bench_stdio.dat bench_sys.dat

test: $(TARGET)
	@echo "\n--- [1] PREPARANDO ENTORNO DE PRUEBA ---"
	mkdir -p src_test/subcarpeta
	echo "Hola, este es un archivo de prueba 1" > src_test/archivo1.txt
	echo "Esto es otra prueba" > src_test/archivo2.txt
	echo "int main(void) { return 0; }" > src_test/main.c
	echo "Archivo en subcarpeta" > src_test/subcarpeta/archivo3.txt
	echo "#include <stdio.h>" > src_test/subcarpeta/archivo4.c
	echo "Archivo simple" > test_file.txt

	@echo "\n--- [2] PROBANDO BACKUP DE ARCHIVO SIMPLE ---"
	./$(TARGET) -b test_file.txt test_file_backup.txt
	@test -f test_file_backup.txt && echo "[EXITO] Archivo copiado" || (echo "[FALLO] Archivo no existe" && exit 1)
	@cmp test_file.txt test_file_backup.txt && echo "[EXITO] Contenido identico" || (echo "[FALLO] Contenido diferente" && exit 1)

	@echo "\n--- [3] PROBANDO BACKUP DE DIRECTORIO COMPLETO ---"
	./$(TARGET) -b src_test dest_test --all

	@echo "\n--- [4] VERIFICANDO ESTRUCTURA DE DIRECTORIO COMPLETA ---"
	@test -f dest_test/archivo1.txt && echo "[EXITO] archivo1.txt copiado" || (echo "[FALLO] archivo1.txt no existe" && exit 1)
	@test -f dest_test/main.c && echo "[EXITO] main.c copiado" || (echo "[FALLO] main.c no existe" && exit 1)
	@test -f dest_test/subcarpeta/archivo3.txt && echo "[EXITO] subcarpeta/archivo3.txt copiado" || (echo "[FALLO] subcarpeta/archivo3.txt no existe" && exit 1)
	@test -f dest_test/subcarpeta/archivo4.c && echo "[EXITO] subcarpeta/archivo4.c copiado" || (echo "[FALLO] subcarpeta/archivo4.c no existe" && exit 1)

	@echo "\n--- [5] PROBANDO FILTRO POR EXTENSION (.c) ---"
	./$(TARGET) -b src_test dest_test_c -e .c
	@test -f dest_test_c/main.c && echo "[EXITO] main.c filtrado correctamente" || (echo "[FALLO] main.c no fue copiado" && exit 1)
	@test -f dest_test_c/subcarpeta/archivo4.c && echo "[EXITO] archivo4.c filtrado correctamente" || (echo "[FALLO] archivo4.c no fue copiado" && exit 1)
	@test ! -f dest_test_c/archivo1.txt && echo "[EXITO] archivo1.txt omitido por filtro" || (echo "[FALLO] archivo1.txt no debio copiarse" && exit 1)
	@test ! -f dest_test_c/subcarpeta/archivo3.txt && echo "[EXITO] archivo3.txt omitido por filtro" || (echo "[FALLO] archivo3.txt no debio copiarse" && exit 1)

	@echo "\n--- [6] PRUEBAS COMPLETADAS EXITOSAMENTE ---"

.PHONY: all clean test
