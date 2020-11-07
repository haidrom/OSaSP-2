#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>

DWORD fGetPID(TCHAR* szProcessName);
BOOL DoRtlAdjustPrivilege();
char path[] = "D:\\MemoryChanger.dll";
TCHAR procName[] = TEXT("InjectImportTest.exe");

int main()
{
    HMODULE hKernel = LoadLibrary(TEXT("Kernel32.dll"));
    auto loadLib = GetProcAddress(hKernel, "LoadLibraryA");
    DWORD pid = fGetPID(procName);
    if (pid == 0 || /*!DoRtlAdjustPrivilege() ||*/ !loadLib) return -1;
    std::cout << "Proccess pid:" << pid << std::endl;
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    if (!hProc) return -1;
    LPVOID memAlloc = VirtualAllocEx(hProc, nullptr, strlen(path) + 1, MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE);
    if (WriteProcessMemory(hProc, memAlloc, path, strlen(path) + 1, nullptr))
    {
         HANDLE HThread = CreateRemoteThread(hProc, nullptr, 0, (LPTHREAD_START_ROUTINE)loadLib, (LPVOID)memAlloc, 0, nullptr);

            if (!HThread) {
                printf("error\n");
            }
            else {
                WaitForSingleObject(HThread, INFINITE);
                std::cout << "Injected" << std::endl;
                CloseHandle(HThread);
            }
    }
    VirtualFreeEx(hProc, memAlloc, 0, MEM_RELEASE);
    CloseHandle(hProc);
    FreeLibrary(hKernel);
}

DWORD fGetPID(TCHAR* szProcessName) {
    PROCESSENTRY32 pe = {
      sizeof(PROCESSENTRY32)
    };
    HANDLE hSS;
    DWORD pid = 0;

    hSS = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSS) {
        if (Process32First(hSS, &pe))
            do 
            {
                if (!_tcscmp(pe.szExeFile, szProcessName)) {
                    pid = pe.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSS, &pe));
        CloseHandle(hSS);
    }
    return pid;
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