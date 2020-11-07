#include <iostream>
#include <Windows.h>
#include <string>

std::string pattern = "Hello world",
newValue = "dlrow olleH";

typedef int changeMemory(std::string pattern, std::string newValue, unsigned int pId);

int main()
{
	HINSTANCE hLib = LoadLibrary(TEXT("../../MemoryChanger/Debug/MemoryChanger.dll"));
	if (!hLib) {
		std::cerr << "Cannot load Dll." << std::endl;
		return -1;
	}
	changeMemory* pChangeMem = (changeMemory*)GetProcAddress(hLib, "changeMemory");
	std::cout << pattern << std::endl;
	Sleep(1000);
	pChangeMem(pattern, newValue, (unsigned long int)GetCurrentProcessId());
	std::cout << pattern << std::endl;
	FreeLibrary(hLib);
}