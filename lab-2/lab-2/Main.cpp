#include <windows.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

const LPCWSTR className = L"MyClass";

const int colCount = 3;

const string filePath = "text.txt";

const POINT min = { 500, 800 }, max = { 1500, 1000 };

vector<string> text;


//message processing
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//reading test file
bool readFile();

//table drawing
void drawLine(HDC hdc, int x1, int y1, int x2, int y2);
void drawVerticalLines(HDC hdc, int width, int height);
void drawTable(HWND hWnd, WPARAM wParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc; 
    HWND hWnd; 
    MSG msg;

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = className;

    RegisterClass(&wc);

    hWnd = CreateWindow(
        className, 
        L"Laba 2", 
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        500, 
        800, 
        HWND_DESKTOP, 
        NULL, 
        hInstance, 
        NULL
    );

    if (!readFile()) return 0;

    ShowWindow(hWnd, nCmdShow);

    BOOL bRet;
    while ((bRet = GetMessage(&msg, hWnd, 0, 0)) != 0)
    {
        if (bRet == -1)
        {
            return (int)msg.wParam;
        }
        else
        {
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        case WM_GETMINMAXINFO:
        {
            MINMAXINFO* minmaxPtr;
            minmaxPtr = (MINMAXINFO*)lParam;
            minmaxPtr->ptMinTrackSize = min;
            minmaxPtr->ptMaxTrackSize = max;
            break;
        }
        case WM_PAINT:
        {
            drawTable(hWnd, wParam);
            break;
        }
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

bool readFile()
{
    ifstream fin;
    fin.open(filePath);
    if (!fin.is_open()) return false;
    string str;
    while (getline(fin, str)) text.push_back(str);
    fin.close();
    return true;
}

void drawLine(HDC hdc, int x1, int y1, int x2, int y2)
{
    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);
}

void drawVerticalLines(HDC hdc, int width, int height)
{
    for (int i = 1; i < colCount; i++)
    {
        drawLine(hdc, i * width, 0, i * width, height);
    }
}

void drawTable(HWND hWnd, WPARAM wParam)
{
    RECT textRect;
    if (!GetClientRect(hWnd, &textRect)) GetLastError();

    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    int wndHeight = textRect.bottom;
    int wndWidth = textRect.right;
    int colWidth = wndWidth / colCount;

    drawVerticalLines(hdc, colWidth, wndHeight);

    textRect.top = 5;

    for (int cellIndex = 0; cellIndex < text.size(); )
    {
        int maxTextHeight = 0;

        for (int col = 0; col < colCount && cellIndex < text.size(); col++) {

            textRect.left = col * colWidth + 5;
            textRect.right = (col + 1) * colWidth - 5;

            const CHAR* cellText = text[cellIndex++].c_str();

            int currTextHeight = DrawTextA(hdc, (LPCSTR)cellText, strlen(cellText), &textRect, DT_VCENTER | DT_WORDBREAK | DT_NOCLIP);

            maxTextHeight = (currTextHeight > maxTextHeight) ? currTextHeight : maxTextHeight;
        }
        textRect.top += maxTextHeight + 10;
        if (cellIndex < text.size()) drawLine(hdc, 0, textRect.top - 5, wndWidth, textRect.top - 5);
    }

    EndPaint(hWnd, &ps);
}