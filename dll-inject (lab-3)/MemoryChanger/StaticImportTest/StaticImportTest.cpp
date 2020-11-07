#include <MemoryChanger.h>
#include <iostream>
#include <Windows.h>
#include <string>

std::string pattern = "Hello world",
newValue = "dlrow olleH";

int main()
{
	std::cout << pattern << std::endl;
	Sleep(1000);
	changeMemory(pattern, newValue, (unsigned long int)GetCurrentProcessId());
	std::cout << pattern << std::endl;
}