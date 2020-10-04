#include <windows.h>
#include <gdiplus.h>

#define IDT_TIMER 12345

const LPCWSTR className = L"MyClass";
const int height = 100;
const int width = 100;
const int step = 5;
const POINT min = { 200, 200 }, max = { 800, 800 };
int animationStepX = step;
int animationStepY = step;
int x = 0, y = 0;
bool animation = false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void redrawWindow(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    InvalidateRect(hWnd, &rect, true);
}

void stopAnimation(HWND hWnd)
{
    animation = false;
    KillTimer(hWnd, IDT_TIMER);
}

void startAnimation(HWND hWnd)
{
    animation = true;
    SetTimer(hWnd, IDT_TIMER, 40, NULL);
}

bool moveRect(HWND hWnd, bool direction, int value)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    if (direction)
    {
        int oldY = y;
        y += (y + value >= 0 && y + height + value <= rect.bottom) ? value : 0;
        redrawWindow(hWnd);
        return y != oldY;
    }
    else
    {
        int oldX = x;
        x += (x + value >= 0 && x + width + value <= rect.right) ? value : 0;
        redrawWindow(hWnd);
        return x != oldX;
    }
}

void onKeyDown(HWND hWnd, WPARAM wParam)
{
    switch (wParam)
    {
    case VK_UP:
    {
        stopAnimation(hWnd);
        moveRect(hWnd, true, -step);
        break;
    }
    case VK_DOWN:
    {
        stopAnimation(hWnd);
        moveRect(hWnd, true, step);
        break;
    }
    case VK_LEFT:
    {
        stopAnimation(hWnd);
        moveRect(hWnd, false, -step);
        break;
    }
    case VK_RIGHT:
    {
        stopAnimation(hWnd);
        moveRect(hWnd, false, step);
        break;
    }
    }
}

void onMouseWheel(HWND hWnd, WPARAM wParam)
{
    bool direction = (GetKeyState(VK_SHIFT) >= 0);
    int moveStep = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? step : -step;
    moveRect(hWnd, direction, moveStep);
}

void onPaint(HWND hWnd, WPARAM wParam)
{
    RECT rect;
    if (!GetClientRect(hWnd, &rect)) GetLastError();
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    HDC  memDC = CreateCompatibleDC(hdc);
    HBITMAP hBM = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
    HBITMAP mOldBmp = (HBITMAP)SelectObject(memDC, hBM);

    Gdiplus::Graphics graphics(memDC);
    Gdiplus::Image image(L"C:\\Users\\Mi\\Downloads\\Zhrumen.png");
    Gdiplus::Rect destRect(x, y, height, width);
    graphics.DrawImage(&image, destRect);

    BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);
    SelectObject(memDC, mOldBmp);
    DeleteObject(hBM);
    DeleteDC(memDC);
    EndPaint(hWnd, &ps);
}

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
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 4);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = className;

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    RegisterClass(&wc);

    hWnd = CreateWindow(className, L"Laba 1", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, HWND_DESKTOP, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    BOOL bRet;
    while ((bRet = GetMessage(&msg, hWnd, 0, 0)) != 0)
    {
        if (bRet == -1)
        {
            Gdiplus::GdiplusShutdown(gdiplusToken);
            return (int)msg.wParam;
        }
        else
        {
            DispatchMessage(&msg);
        }
    }
    Gdiplus::GdiplusShutdown(gdiplusToken);
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_ERASEBKGND:
            return TRUE;
        case WM_LBUTTONDOWN:
        {
            animation ? stopAnimation(hWnd) : startAnimation(hWnd);
            break;
        }
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
        case WM_TIMER:
        {
            if (!moveRect(hWnd, true, animationStepY)) animationStepY = -animationStepY;
            if (!moveRect(hWnd, false, animationStepX)) animationStepX = -animationStepX;
            break;
        }
        case WM_PAINT:
        {
            onPaint(hWnd, wParam);
            break;
        }
        case WM_MOUSEWHEEL:
        {
            stopAnimation(hWnd);
            onMouseWheel(hWnd, wParam);
            break;
        }
        case WM_KEYDOWN:
        {   
            onKeyDown(hWnd, wParam);
            break;
        }
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}