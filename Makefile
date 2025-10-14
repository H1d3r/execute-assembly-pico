libtcg: lib/libtcg/
	cd lib/libtcg && make all
	
pico: src/execute_assembly.c src/headers/
	mkdir -p bin out
	x86_64-w64-mingw32-gcc -DWIN_X64 -shared -masm=intel -Wall -Wno-pointer-arith -fno-toplevel-reorder -c src/execute_assembly.c -o bin/execute_assembly.o
	
runner: loader.spec crystal-palace/ src/runner.c
	make libtcg
	make pico
	x86_64-w64-mingw32-gcc -DWIN_X64 -shared -masm=intel -Wall -Wno-pointer-arith -fno-toplevel-reorder -c src/runner.c -o bin/runner.o
	java -Dcrystalpalace.verbose=false -classpath crystal-palace/crystalpalace.jar crystalpalace.spec.LinkSpec buildPic ./loader.spec x64 out/runner.bin
