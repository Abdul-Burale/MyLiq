#include <windows.h>
#include <stdbool.h>
#include <GL/GL.h>
#include <GL/GLU.h>

HDC CreateDeviceContextz(HWND Window);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);