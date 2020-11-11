#pragma once

#include <Windows.h>
#include <list>

#define DEFAULT_THREAD_COUNT 10

class ThreadPool
{
	static DWORD WINAPI StartThread(LPVOID lpParam);

private: 
	bool _isWorking = true;
	unsigned long int _currentTasksCount = 0;
	unsigned long int _workingThreadsCount = 0;
	std::list<void (*)(LPVOID lpParam)> _tasksQueue;
	std::list<LPVOID> _params;
	HANDLE _threads[DEFAULT_THREAD_COUNT] = {};
	HANDLE _emptyTasksQueueEvent;
	HANDLE _mutex;

	DWORD WINAPI ThreadLoop();
	void WaitAll();
public:

	ThreadPool();
	~ThreadPool();

	void AddTask(void (*task)(LPVOID lpParam), LPVOID lpParam);
	void Wait();
};
