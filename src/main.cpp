#include <cstdint>
#include <windows.h>

#define global_variable static
#define local_variable static

global_variable bool Running = 0;
global_variable BITMAPINFO bitMapInfo;
global_variable void *bitMapMemory;

global_variable int bitMapWidth;
global_variable int bitMapHeight;

void Win32ResizeDIBSection(int width, int height) {

  if (bitMapMemory) {
    VirtualFree(bitMapMemory, 0, MEM_RELEASE);
  }

  bitMapWidth = width;
  bitMapHeight = height;
  int bytesPerPixel = 4;
  int bitMapMemorySize = bitMapWidth * bitMapHeight * bytesPerPixel;

  bitMapInfo.bmiHeader.biSize = sizeof(bitMapInfo.bmiHeader);
  bitMapInfo.bmiHeader.biWidth = bitMapWidth;
  bitMapInfo.bmiHeader.biHeight = -bitMapHeight;
  bitMapInfo.bmiHeader.biPlanes = 1;
  bitMapInfo.bmiHeader.biBitCount = 32;
  bitMapInfo.bmiHeader.biCompression = BI_RGB;

  bitMapMemory = VirtualAlloc(0, bitMapMemorySize, MEM_RESERVE | MEM_COMMIT,
                              PAGE_READWRITE);

  uint8_t *row = (uint8_t *)bitMapMemory;
  int pitch = bitMapWidth * bytesPerPixel;
  for (int y = 0; y < bitMapHeight; ++y) {
    uint8_t *pixel = (uint8_t *)row;
    for (int x = 0; x < bitMapWidth; ++x) {

      // pixel in memory
      // BB GG RR xx

      *pixel = 0;
      ++pixel;

      *pixel = 255;
      ++pixel;

      *pixel = 0;
      ++pixel;

      *pixel = 0;
      ++pixel;
    }
    row += pitch;
  }
}

void Win32UpdateWindow(HDC hdc, RECT *clientRect) {
  int windowWidth = clientRect->right - clientRect->left;
  int windowHeight = clientRect->bottom - clientRect->top;
  StretchDIBits(hdc, 0, 0, windowWidth, windowHeight, 0, 0, bitMapWidth,
                bitMapHeight, bitMapMemory, &bitMapInfo, DIB_RGB_COLORS,
                SRCCOPY);
}

LRESULT Win32Wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  LRESULT result = 0;

  switch (uMsg) {

  case WM_SIZE: {
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;
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

    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    Win32UpdateWindow(hdc, &clientRect);

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
