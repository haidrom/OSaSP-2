#include "ThreadPool.h"
#include <iostream>


ThreadPool::ThreadPool()
{
	DWORD dwThreadId = 1;
	_emptyTasksQueueEvent = CreateEvent(NULL, true, false, NULL);
	_mutex = CreateMutex(NULL, false, NULL);
	for (int i = 0; i < DEFAULT_THREAD_COUNT; i++)
	{
		HANDLE hThread = CreateThread(
			NULL, 
			0, 
			StartThread,
			(void*) this, 
			0, 
			&dwThreadId
		);
		_threads[i] = hThread;
	}
}


DWORD WINAPI ThreadPool::StartThread(LPVOID lpParam)
{
	ThreadPool* instance = (ThreadPool*)lpParam;
	return instance->ThreadLoop();
}


DWORD WINAPI ThreadPool::ThreadLoop()
{
	bool flag = false;
	while (_isWorking)
	{
		void (*task)() = NULL;
		WaitForSingleObject(_mutex, INFINITE);
		{
			flag = _currentTasksCount == 0;
			if (!flag)
			{
				task = _tasksQueue.front();
				_tasksQueue.pop_front();
				_workingThreadsCount++;
				_currentTasksCount--;
			}
		}
		ReleaseMutex(_mutex);
		if (flag) (WaitForSingleObject(_emptyTasksQueueEvent, INFINITE));
		else
		{
			task();
			InterlockedDecrement(&_workingThreadsCount);
		}
	}
	ExitThread(0);
}


ThreadPool::~ThreadPool()
{
	WaitAll();
	WaitForMultipleObjects(DEFAULT_THREAD_COUNT, _threads, true, INFINITE);
	for (int i = 0; i < DEFAULT_THREAD_COUNT; i++)
	{
		CloseHandle(_threads[i]);
	}
	CloseHandle(_mutex);
	CloseHandle(_emptyTasksQueueEvent);
}


void ThreadPool::AddTask(void (*task)())
{
	WaitForSingleObject(_mutex, INFINITE);
	if (_isWorking)
	{
		_tasksQueue.push_back(task);
		_currentTasksCount++;
		SetEvent(_emptyTasksQueueEvent);
		ResetEvent(_emptyTasksQueueEvent);
	}
	ReleaseMutex(_mutex);
}

void ThreadPool::WaitAll()
{
	bool flag = false;
	while (!flag)
	{
		WaitForSingleObject(_mutex, INFINITE);
		{
			flag = _tasksQueue.size() == 0 && _workingThreadsCount == 0;
			if (flag)
			{
				_isWorking = false;
				SetEvent(_emptyTasksQueueEvent);
				_tasksQueue;
			}
		}
		ReleaseMutex(_mutex);
	}
}
