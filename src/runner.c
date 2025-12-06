#include <windows.h>
#include "execute_assembly.h"
#include "headers/tcg.h"

WCHAR __CMDLINE__[1024] __attribute__((section(".rdata")));

WINBASEAPI HANDLE WINAPI KERNEL32$GetModuleHandleA(LPCSTR lpModuleName);
WINBASEAPI HMODULE WINAPI KERNEL32$LoadLibraryA(LPCSTR lpLibFileName);
WINBASEAPI LPVOID WINAPI KERNEL32$GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
WINBASEAPI LPVOID WINAPI KERNEL32$VirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
WINBASEAPI BOOL WINAPI KERNEL32$VirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD  dwFreeType);
WINBASEAPI LPWSTR * WINAPI SHELL32$CommandLineToArgvW(LPCWSTR lpCmdLine, int *pNumArgs);

FARPROC resolve(DWORD modHash, DWORD funcHash) {
    HANDLE hModule = findModuleByHash(modHash);
    return findFunctionByHash(hModule, funcHash);
}

FARPROC resolve_unloaded(char * mod, char * func) {
  HANDLE hModule = KERNEL32$GetModuleHandleA(mod);
  if (hModule == NULL) {
    hModule = KERNEL32$LoadLibraryA(mod);
  }
  return KERNEL32$GetProcAddress(hModule, func);
}

typedef struct {
    __typeof__(LoadLibraryA)   * LoadLibraryA;
    __typeof__(GetProcAddress) * GetProcAddress;
    __typeof__(VirtualAlloc)   * VirtualAlloc;
    __typeof__(VirtualFree)    * VirtualFree;
} WIN32FUNCS;

typedef struct {
    int   len;
    char  buf[];
} _ASSEMBLY;

char __PICODATA__[0] __attribute__((section("my_pico")));
char __DLLDATA__[0] __attribute__((section("my_assembly")));
  
char * findAppendedPICO() {
    return (char *)&__PICODATA__;
}

_ASSEMBLY * findAppendedAssembly() {
    return (_ASSEMBLY *)&__DLLDATA__;
}

void run_clr_pico(WIN32FUNCS * funcs, char * srcPico, WCHAR *assemblyArgs[], size_t assemblyArgLen) {
    char        * dstCode;
    char        * dstData;
     
    /* allocate memory for our PICO */
    dstData = funcs->VirtualAlloc( NULL, PicoDataSize(srcPico), MEM_RESERVE|MEM_COMMIT|MEM_TOP_DOWN,
                                                PAGE_READWRITE );
    dstCode = funcs->VirtualAlloc( NULL, PicoCodeSize(srcPico), MEM_RESERVE|MEM_COMMIT|MEM_TOP_DOWN,
                                            PAGE_EXECUTE_READWRITE );
  
    /* load our pico into our destination address, thanks! */
    PicoLoad((IMPORTFUNCS *)funcs, srcPico, dstCode, dstData);
  
    /* And, we can call our pico entry point */
    EXECUTE_ASSEMBLY_PICO entryPoint = (EXECUTE_ASSEMBLY_PICO) PicoEntryPoint(srcPico, dstCode);
    
    _ASSEMBLY *assembly = findAppendedAssembly();
    
    HRESULT result = entryPoint(assembly->buf, assembly->len, assemblyArgs, assemblyArgLen);
    //dprintf("Execute assembly result: 0x%x\n", result);
  
    /* free everything... */
    funcs->VirtualFree(dstData, 0, MEM_RELEASE);
    funcs->VirtualFree(dstCode, 0, MEM_RELEASE);
}

void go() {
	// Resolve necessary WIN32 APIs.
	WIN32FUNCS funcs;
	funcs.LoadLibraryA = KERNEL32$LoadLibraryA;
	funcs.GetProcAddress = KERNEL32$GetProcAddress;
	funcs.VirtualAlloc = KERNEL32$VirtualAlloc;
	funcs.VirtualFree = KERNEL32$VirtualFree;
	
	// Get a pointer to the section containing our PICO.
	char *pico = findAppendedPICO();
	
	// Arguments to pass to the assembly.
	int argc;
	LPCWSTR cmdline = (LPCWSTR)&__CMDLINE__;
	LPWSTR *argv = SHELL32$CommandLineToArgvW(cmdline, &argc);
	
	// Run the PICO.
	run_clr_pico(&funcs, pico, argv, argc);
}
