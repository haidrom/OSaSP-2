// lab4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "ThreadPool.h"

void task();
void anotherTask();

int main()
{
    std::cout << "Hello World!\n";
    ThreadPool threadPool = ThreadPool();
    threadPool.AddTask(task);
    threadPool.AddTask(anotherTask);
   // Sleep(5000);
}

void task()
{
    std::cout << "task" << std::endl;
}

void anotherTask()
{
    std::cout << "anothertask" << std::endl;
}
