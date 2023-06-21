#include <stdio.h>
#include <stdint.h>
#include "render.h"

static bool Running = true; 

static void Win32InitOpenGL(HWND Window)
{
    HDC WindowDC = GetDC(Window);
    PIXELFORMATDESCRIPTOR Suggested_Pixel_Format = {0};

    PIXELFORMATDESCRIPTOR PixelFormat = {0};
    PixelFormat.nSize = sizeof(PixelFormat);
    PixelFormat.nVersion = 1;
    PixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    PixelFormat.cColorBits = 32;
    PixelFormat.cAlphaBits = 8;

    int Suggested_Pixel_Format_IDX = ChoosePixelFormat(WindowDC, &PixelFormat);
    DescribePixelFormat(WindowDC, Suggested_Pixel_Format_IDX, sizeof(Suggested_Pixel_Format), &Suggested_Pixel_Format);
    SetPixelFormat(WindowDC, Suggested_Pixel_Format_IDX, &Suggested_Pixel_Format);

    HGLRC OpenGLDC = wglCreateContext(WindowDC);
    if(wglMakeCurrent(WindowDC, OpenGLDC))
    {
        glViewport(0, 0, 1500, 750);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
       
        SwapBuffers(WindowDC);
    }
    else
    {

    }
    ReleaseDC(Window, WindowDC);

}

static void CheckMessage()
{
    MSG Message;

    while(PeekMessage(&Message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
}

int CALLBACK WinMain(HINSTANCE Instance,
  HINSTANCE hPrevInstance,
  LPSTR     lpCmdLine,
  int       nShowCmd)
{
    WNDCLASS _WClass = {0};

    _WClass.style = CS_VREDRAW | CS_HREDRAW;
    _WClass.lpfnWndProc = WindowProc;
    _WClass.hInstance = Instance;
    _WClass.lpszClassName = "Test Class";

    if(RegisterClass(&_WClass)){
        HWND Window = CreateWindowEx(0, _WClass.lpszClassName, "Test Window",
                                     WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
                                     CW_USEDEFAULT, CW_USEDEFAULT, 0 , 0, Instance, 0);

    if(Window == NULL)
    {
        return 0;
    }
    
    Win32InitOpenGL(Window);
    
    HDC WinDC = GetDC(Window);
    while(Running)
    {
        CheckMessage();

    }

    
    }


}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    LRESULT Result = 0;
    switch(uMsg)
    {   
        
        case WM_CREATE:
        {
            printf("Window Created!\n");
           // print("%d Ms\n");
        } break;
        
        //TODO: WM_KEYDOWN | WM_KEYUP used to log user input to config
        case WM_SIZE:
        {
            return 0;
        } break;
        
        case WM_PAINT:
        {
            
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(hwnd, &Paint);

            FillRect(DeviceContext, &Paint.rcPaint, CreateSolidBrush(RGB(0, 255, 0)));
            EndPaint(hwnd, &Paint);
            return 0;
            
        } break;

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            printf("WM_DESTROY\n");
            Running = false;
            return 0;
        } break;

        case WM_CLOSE:
        {
            printf("WM_CLOSE\n");
            DestroyWindow(hwnd);
            Running = false;
            return 0;
        } break;

        default:
        {
            Result = DefWindowProc(hwnd, uMsg, wParam, lParam);
        } break;
    }
    
    return Result;
}


