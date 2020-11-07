#include "pch.h"
#include "MemoryChanger.h"
#include <string>
#include <vector>
#include <algorithm>

#define WRITABLE (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)

std::vector<char> buffer;

BOOL DoRtlAdjustPrivilege();

int changeMemory(std::string pattern, std::string newValue, unsigned long int pId)
{
    int changesCount = 0;
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, false, pId);
    if (!hProc /*|| !DoRtlAdjustPrivilege()*/) return -1;
    unsigned char* baseAddr = nullptr;
    MEMORY_BASIC_INFORMATION mbi;
    for (; VirtualQueryEx(hProc, baseAddr, &mbi, sizeof(mbi)); baseAddr += mbi.RegionSize)
    {
        if ((mbi.State == MEM_COMMIT) && (mbi.Protect & WRITABLE))
        {
            SIZE_T numOfBytes;
            buffer.resize(mbi.RegionSize);
            ReadProcessMemory(hProc, baseAddr, buffer.data(), mbi.RegionSize, &numOfBytes);
            buffer.resize(numOfBytes);
            auto iterator = std::search(buffer.begin(), buffer.end(), pattern.begin(), pattern.end());
            if (iterator != buffer.end())
            {
                LPVOID addr = baseAddr + std::distance(buffer.begin(), iterator);
                if (!WriteProcessMemory(hProc, addr, (LPCVOID)newValue.data(), newValue.length() + 1, NULL))
                    return -2;
                else
                    break;
            }
        }
        
    }
    CloseHandle(hProc);
    return changesCount;
}

BOOL DoRtlAdjustPrivilege() {
#define SE_DEBUG_PRIVILEGE 20L
#define AdjustCurrentProcess 0
    BOOL bPrev = FALSE;
    LONG(WINAPI * RtlAdjustPrivilege)(DWORD, BOOL, INT, PBOOL);
    *(FARPROC*)&RtlAdjustPrivilege = GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "RtlAdjustPrivilege");
    if (!RtlAdjustPrivilege) return FALSE;
    RtlAdjustPrivilege(SE_DEBUG_PRIVILEGE, TRUE, AdjustCurrentProcess, &bPrev);
    return TRUE;
}
