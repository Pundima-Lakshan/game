#include <windows.h>

LRESULT Wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  LRESULT result = 0;
  switch (uMsg) {
  case WM_SIZE: {
    OutputDebugStringA("WM_SIZE\n");
  } break;

  case WM_DESTROY: {
    OutputDebugStringA("WM_DESTROY\n");
  } break;

  case WM_CLOSE: {
    OutputDebugStringA("WM_CLOSE\n");
  } break;

  case WM_ACTIVATEAPP: {
    OutputDebugStringA("WM_ACTIVATEAPP\n");
  } break;

  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

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
  wc.lpfnWndProc = Wndproc;
  wc.hInstance = hInst;
  // HICON     hIcon;
  wc.lpszClassName = "GameWindowClass";

  if (RegisterClassA(&wc)) {
    HWND hWnd = CreateWindowEx(0, wc.lpszClassName, "Game",
                               WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT,
                               CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0,
                               0, hInst, 0);

    if (hWnd) {
      MSG msg = {};
      while (GetMessageA(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    } else {
      return -1;
    }
  }

  return 0;
}
