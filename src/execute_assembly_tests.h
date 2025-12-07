#include "headers/cpltest.h"

void test_get_clr() {
	HRESULT result;
	AssemblyData assemblyData = { 0 };
	result = get_clr(&assemblyData);
	ASSERT(result == S_OK, "test_get_clr: Could not retrieve the CLR.");
}

void test_get_runtime() {
	HRESULT result;
	AssemblyData assemblyData = { 0 };
	result = get_clr(&assemblyData);
	ASSERT(result == S_OK, "test_get_runtime: Could not retrieve the CLR.");
	result = get_runtime(&assemblyData);
	ASSERT(result == S_OK, "test_get_runtime: Could not retrieve the runtime interface.");
}

void test_load_runtime() {
	HRESULT result;
	AssemblyData assemblyData = { 0 };
	result = get_clr(&assemblyData);
	ASSERT(result == S_OK, "test_load_runtime: Could not retrieve the CLR.");
	result = get_runtime(&assemblyData);
	ASSERT(result == S_OK, "test_load_runtime: Could not retrieve the runtime interface.");
	result = load_runtime(&assemblyData);
	ASSERT(result == S_OK, "test_load_runtime: Could not load and initialise the hosted CLR.");
}

void test_get_default_appdomain() {
	HRESULT result;
	AssemblyData assemblyData = { 0 };
	result = get_clr(&assemblyData);
	ASSERT(result == S_OK, "test_get_default_appdomain: Could not retrieve the CLR.");
	result = get_runtime(&assemblyData);
	ASSERT(result == S_OK, "test_get_default_appdomain: Could not retrieve the runtime interface.");
	result = load_runtime(&assemblyData);
	ASSERT(result == S_OK, "test_get_default_appdomain: Could not load and initialise the hosted CLR.");
	result = get_default_appdomain(&assemblyData);
	ASSERT(result == S_OK, "test_get_default_appdomain: Could not retrieve the default AppDomain.");
}

HRESULT execute_assembly_tests() {
	TESTFUNCS tests = initTests();
	addTest(&tests, test_get_clr);
	addTest(&tests, test_get_runtime);
	addTest(&tests, test_load_runtime);
	addTest(&tests, test_get_default_appdomain);
	runTests(&tests);
	freeTests(&tests);
	return S_OK;
}
