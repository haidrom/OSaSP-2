// lab4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <list>
#include <vector>
#include <fstream>
#include <string>
#include <iterator>
#include "ThreadPool.h"

void sortTwoVectors(LPVOID lpParam);

int main()
{
    std::ifstream file("D:\\test.txt");
    std::vector<std::string> fileLines;
    std::string line;
    if (file.is_open())
    {
        while (getline(file, line))
        {
            fileLines.push_back(line);
        }
    }
    file.close();
    copy(fileLines.begin(), fileLines.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
    std::cout << std::endl;
    int partsSize = 2;
    while (partsSize <= fileLines.size())
    {
        std::vector<std::vector<std::string>> parts;
        ThreadPool threadPool = ThreadPool();
        int i = 0;
        while (i < fileLines.size())
        {
            std::vector<std::string> buf(partsSize);
            int j = 0;
            while (j < partsSize)
            {
                buf[j++] = fileLines[i++];
            }
            parts.push_back(buf);
        }
        for (int i = 0; i < parts.size(); i++)
        {
            threadPool.AddTask(sortTwoVectors, &parts[i]);
        }
        threadPool.Wait();
        for (int i = 0; i < parts.size(); i++)
        {
            copy((parts[i]).begin(), (parts[i]).end(), fileLines.begin() + i * partsSize);
        }
        parts.clear();
        partsSize *= 2;
    }
    copy(fileLines.begin(), fileLines.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
    std::cout << std::endl;
  
}

void sortTwoVectors(LPVOID lpParam)
{
    std::vector<std::string>* vec = (std::vector<std::string>*)lpParam;
    int size = vec->size();
    int mid = size / 2;
    int i = 0, j = mid;
    std::vector<std::string> buf;
    while (i < mid && j < size)
    {
        vec->at(i) < vec->at(j) ? buf.push_back(vec->at(i++)) : buf.push_back(vec->at(j++));
    }
    if (i >= mid)
    {
        while (j < size)
        {
            buf.push_back(vec->at(j++));
        }
    }
    else
    {
        while (i < mid)
        {
            buf.push_back(vec->at(i++));
        }
    }
    *vec = buf;
}
