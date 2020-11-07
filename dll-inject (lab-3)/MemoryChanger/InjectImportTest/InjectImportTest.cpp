#include <iostream>
#include <Windows.h>


std::string pattern = "Hello world",
newValue = "dlrow olleH";

int main()
{
    while (true)
    {
        std::cout << GetCurrentProcessId() << std::endl;
        std::cout << pattern << std::endl;
        Sleep(5000);
    }
}
