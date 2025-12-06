dirs:
	mkdir -p bin out

libtcg: lib/libtcg/
	cd lib/libtcg && make all
	
libcpltest: lib/LibCPLTest/
	cd lib/LibCPLTest && make all
	
pico: dirs src/execute_assembly.c src/headers/
	x86_64-w64-mingw32-gcc -DWIN_X64 -shared -masm=intel -Wall -Wno-pointer-arith -fno-toplevel-reorder -c src/execute_assembly.c -o bin/execute_assembly.o
	
runner: dirs libtcg pico crystal-palace/ runner.spec src/runner.c
	x86_64-w64-mingw32-gcc -DWIN_X64 -shared -masm=intel -Wall -Wno-pointer-arith -fno-toplevel-reorder -c src/runner.c -o bin/runner.o
	java -Dcrystalpalace.verbose=false -jar crystal-palace/crystalpalace.jar buildPic ./runner.spec x64 out/runner.bin @config.spec
	
tester: dirs libtcg libcpltest runner crystal-palace/ tester.spec src/runner.c
	x86_64-w64-mingw32-gcc -DCPLTESTS -DWIN_X64 -shared -masm=intel -Wall -Wno-pointer-arith -fno-toplevel-reorder -c src/execute_assembly.c -o bin/execute_assembly.test.o
	java -Dcrystalpalace.verbose=false -jar crystal-palace/crystalpalace.jar buildPic ./tester.spec x64 out/tester.bin @config.spec
	
clean: bin out lib/libtcg/ lib/LibCPLTest/
	rm -f bin/*.o
	rm -f out/*.bin
	cd lib/libtcg && make clean
	cd lib/LibCPLTest && make clean
