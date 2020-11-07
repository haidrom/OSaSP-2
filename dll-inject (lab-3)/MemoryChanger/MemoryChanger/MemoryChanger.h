#pragma once

#ifdef MEMORYCHANGER_EXPORTS
#define MEMORYCHANGER_API __declspec(dllexport)
#else
#define MEMORYCHANGER_API __declspec(dllimport)
#endif

#include <iostream>

extern "C" MEMORYCHANGER_API int changeMemory(std::string pattern, std::string newValue, unsigned long int pId);