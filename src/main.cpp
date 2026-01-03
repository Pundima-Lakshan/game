#include <cstdint>
#include <windows.h>

#define global_variable static
#define local_variable static

struct win32_offscreen_buffer
{
  int bytesPerPixel;
  BITMAPINFO info;
  void *memory;
  int width;
  int height;
};

struct win32_window_dimensions
{
  int width;
  int height;
};

global_variable bool globalRunning = 0;
global_variable win32_offscreen_buffer buffer;

win32_window_dimensions Win32GetWindowDimension(HWND hWnd)
{
  win32_window_dimensions dimensions;

  RECT clientRect;
  GetClientRect(hWnd, &clientRect);
  dimensions.width = clientRect.right - clientRect.left;
  dimensions.height = clientRect.bottom - clientRect.top;

  return dimensions;
}

void RenderWeirdGradient(win32_offscreen_buffer *buffer,
                         int xOffset,
                         int yOffset)
{
  uint8_t *row = (uint8_t *)buffer->memory;
  int pitch = buffer->width * buffer->bytesPerPixel;
  for (int y = 0; y < buffer->height; ++y)
  {
    uint8_t *pixel = (uint8_t *)row;
    for (int x = 0; x < buffer->width; ++x)
    {

      // pixel in memory
      // BB GG RR xx

      *pixel = (uint8_t)(x + xOffset);
      ++pixel;

      *pixel = (uint8_t)(y + yOffset);
      ++pixel;

      *pixel = 0;
      ++pixel;

      *pixel = 0;
      ++pixel;
    }
    row += pitch;
  }
}

void Win32ResizeDIBSection(win32_offscreen_buffer *buffer,
                           int width,
                           int height)
{
  if (buffer->memory)
  {
    VirtualFree(buffer->memory, 0, MEM_RELEASE);
  }

  buffer->width = width;
  buffer->height = height;
  buffer->bytesPerPixel = 4;

  buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
  buffer->info.bmiHeader.biWidth = buffer->width;
  buffer->info.bmiHeader.biHeight = -(buffer->height);
  buffer->info.bmiHeader.biPlanes = 1;
  buffer->info.bmiHeader.biBitCount = 32;
  buffer->info.bmiHeader.biCompression = BI_RGB;

  int bitMapMemorySize = buffer->width * buffer->height * buffer->bytesPerPixel;
  buffer->memory = VirtualAlloc(0,
                                bitMapMemorySize,
                                MEM_RESERVE | MEM_COMMIT,
                                PAGE_READWRITE);
}

void Win32CopyBufferToWindow(win32_offscreen_buffer *buffer,
                             HDC hdc,
                             int windowWidth,
                             int windowHeight)
{
  StretchDIBits(hdc,
                0,
                0,
                windowWidth,
                windowHeight,
                0,
                0,
                buffer->width,
                buffer->height,
                buffer->memory,
                &buffer->info,
                DIB_RGB_COLORS,
                SRCCOPY);
}

LRESULT Win32Wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  LRESULT result = 0;

  switch (uMsg)
  {

  case WM_SIZE:
  {
  }
  break;

  case WM_DESTROY:
  {
    globalRunning = false;
    OutputDebugStringA("WM_DESTROY\n");
  }
  break;

  case WM_CLOSE:
  {
    globalRunning = false;
    OutputDebugStringA("WM_CLOSE\n");
  }
  break;

  case WM_ACTIVATEAPP:
  {
    OutputDebugStringA("WM_ACTIVATEAPP\n");
  }
  break;

  case WM_PAINT:
  {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    win32_window_dimensions dimensions = Win32GetWindowDimension(hWnd);

    Win32CopyBufferToWindow(&buffer, hdc, dimensions.width, dimensions.height);

    EndPaint(hWnd, &ps);
  }
  break;

  default:
  {
    result = DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
  break;
  }

  return result;
}

int APIENTRY WinMain(HINSTANCE hInst,
                     HINSTANCE hInstPrev,
                     PSTR cmdline,
                     int cmdshow)
{
  WNDCLASS wc = {};

  Win32ResizeDIBSection(&buffer, 1280, 720);

  wc.style = CS_VREDRAW | CS_HREDRAW;
  wc.lpfnWndProc = Win32Wndproc;
  wc.hInstance = hInst;
  // HICON     hIcon;
  wc.lpszClassName = "GameWindowClass";

  if (RegisterClassA(&wc))
  {
    HWND hWnd = CreateWindowEx(0,
                               wc.lpszClassName,
                               "Game",
                               WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               0,
                               0,
                               hInst,
                               0);

    if (hWnd)
    {
      int xOffset = 0;
      int yOffset = 0;
      globalRunning = true;
      MSG msg = {};
      while (globalRunning)
      {
        while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
        {
          if (msg.message == WM_QUIT)
          {
            globalRunning = false;
          }
          TranslateMessage(&msg);
          DispatchMessage(&msg);
        }

        RenderWeirdGradient(&buffer, xOffset, yOffset);
        ++xOffset;

        HDC hdc = GetDC(hWnd);

        win32_window_dimensions dimensions = Win32GetWindowDimension(hWnd);

        Win32CopyBufferToWindow(&buffer,
                                hdc,
                                dimensions.width,
                                dimensions.height);

        ReleaseDC(hWnd, hdc);
      }
    }
    else
    {
      return -1;
    }
  }

  return 0;
}
