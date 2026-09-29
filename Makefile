CYRSTAL_LINKER	= cpl link
CC_64			= x86_64-w64-mingw32-gcc
CFLAGS			= -O1 -fno-jump-tables -shared -Wall -Wno-pointer-arith

.PHONY: all x64 clean

all: x64

bin:
	mkdir -p bin

x64: bin
	@echo "[*] Compiling..."
	$(CC_64) -DWIN_X64 $(CFLAGS) -Iinclude -c src/services.c -o bin/services.x64.o
	$(CC_64) -DWIN_X64 $(CFLAGS) -Iinclude -c src/dotnet.c -o bin/dotnet.x64.o
	$(CC_64) -DWIN_X64 $(CFLAGS) -Iinclude -c src/loader.c -o bin/loader.x64.o

	@echo "[*] Linking..."
	@$(CYRSTAL_LINKER) $(CURDIR)/loader.spec $(CURDIR)/bin/loader.x64.o $(CURDIR)/bin/execute-assembly.x64.bin \
	%ASSEMBLY_PATH="$(ASSEMBLY_PATH)" %ASSEMBLY_ARGS="$(ASSEMBLY_ARGS)" %PIPE_NAME="$(PIPE_NAME)"
	@echo "[+] Done!"

x64-debug: bin
	@echo "[*] Compiling..."
	$(CC_64) -DWIN_X64 $(CFLAGS) -Iinclude -D_DEBUG -c src/services.c -o bin/services.x64.o
	$(CC_64) -DWIN_X64 $(CFLAGS) -Iinclude -D_DEBUG -c src/dotnet.c -o bin/dotnet.x64.o
	$(CC_64) -DWIN_X64 $(CFLAGS) -Iinclude -D_DEBUG -c src/loader.c -o bin/loader.x64.o

	@echo "[*] Linking..."
	@$(CYRSTAL_LINKER) $(CURDIR)/loader.spec $(CURDIR)/bin/loader.x64.o $(CURDIR)/bin/execute-assembly.x64.bin \
	%ASSEMBLY_PATH="$(ASSEMBLY_PATH)" %ASSEMBLY_ARGS="$(ASSEMBLY_ARGS)" %PIPE_NAME=""
	@echo "[+] Done!"

clean:
	rm -rf bin