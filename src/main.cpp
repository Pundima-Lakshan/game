#include <windows.h>

#define global_variable static
#define local_variable static

global_variable bool Running = 0;
global_variable BITMAPINFO bitMapInfo;
global_variable void *bitMapMemory;
global_variable HBITMAP bitMapHandle;
global_variable HDC bitMapDeviceContext;

void Win32ResizeDIBSection(int width, int height) {
  if (bitMapHandle) {
    DeleteObject(&bitMapHandle);
  }

  if (!bitMapDeviceContext) {
    bitMapDeviceContext = CreateCompatibleDC(0);
  }

  bitMapInfo.bmiHeader.biSize = sizeof(bitMapInfo.bmiHeader);
  bitMapInfo.bmiHeader.biWidth = width;
  bitMapInfo.bmiHeader.biHeight = height;
  bitMapInfo.bmiHeader.biPlanes = 1;
  bitMapInfo.bmiHeader.biBitCount = 32;
  bitMapInfo.bmiHeader.biCompression = BI_RGB;

  CreateDIBSection(bitMapDeviceContext, &bitMapInfo, DIB_RGB_COLORS,
                   &bitMapMemory, 0, 0);
}

void Win32UpdateWindow(HDC hdc, int x, int y, int width, int height) {
  StretchDIBits(hdc, x, y, width, height, x, y, width, height, &bitMapMemory,
                &bitMapInfo, DIB_RGB_COLORS, SRCCOPY);
}

LRESULT Win32Wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  LRESULT result = 0;

  switch (uMsg) {

  case WM_SIZE: {
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    int width = clientRect.right = clientRect.left;
    int height = clientRect.bottom = clientRect.top;
    Win32ResizeDIBSection(width, height);
    OutputDebugStringA("WM_SIZE\n");
  } break;

  case WM_DESTROY: {
    Running = false;
    OutputDebugStringA("WM_DESTROY\n");
  } break;

  case WM_CLOSE: {
    Running = false;
    OutputDebugStringA("WM_CLOSE\n");
  } break;

  case WM_ACTIVATEAPP: {
    OutputDebugStringA("WM_ACTIVATEAPP\n");
  } break;

  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    int x = ps.rcPaint.left;
    int y = ps.rcPaint.top;
    int width = ps.rcPaint.right - ps.rcPaint.left;
    int height = ps.rcPaint.bottom - ps.rcPaint.top;
    Win32UpdateWindow(hdc, x, y, width, height);
    EndPaint(hWnd, &ps);
  } break;

  default: {
    result = DefWindowProc(hWnd, uMsg, wParam, lParam);
  } break;
  }

  return result;
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline,
                     int cmdshow) {
  WNDCLASS wc = {};
  wc.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
  wc.lpfnWndProc = Win32Wndproc;
  wc.hInstance = hInst;
  // HICON     hIcon;
  wc.lpszClassName = "GameWindowClass";

  if (RegisterClassA(&wc)) {
    HWND hWnd = CreateWindowEx(0, wc.lpszClassName, "Game",
                               WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT,
                               CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0,
                               0, hInst, 0);

    if (hWnd) {
      Running = true;
      MSG msg = {};
      while (Running && GetMessageA(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    } else {
      return -1;
    }
  }

  return 0;
}
