#include <windows.h>
#include <stdbool.h>
#include <stdint.h>

#define Locale_var static 
#define Global_var static 


typedef struct Win32_OffScreen_Buffer {
        BITMAPINFO Info;
        void *Memory;
        int Width;
        int Height;
        int Pitch;
        int BytesPerPixel;

} Win32_OffScreen_Buffer;

typedef struct win32_window_dimension {
        int Width;
        int Height;
} Win32_Window_Dimension;

//Global Variables
Global_var bool Running;
Global_var Win32_OffScreen_Buffer GlobalBackBuffer;

Win32_Window_Dimension Win32_GetWindowDimension(HWND Window)
{
    Win32_Window_Dimension Result;
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);

    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;

    return(Result);
}

static void RenderGradient(Win32_OffScreen_Buffer Buffer,int XOffset, int YOffset)
{

    uint8_t *Row = (uint8_t*)Buffer.Memory;
    for (int Y = 0;
         Y < Buffer.Height; Y++)
        {
           uint32_t *Pixel = (uint32_t*)Row; 
           for (int X = 0;
              X < Buffer.Width; X++)
            {
                    int Blue = (X + XOffset);
                    int Green = (Y + YOffset);


                    *Pixel++ = ((Green << 8)  | Blue);
            }
          Row += Buffer.Pitch;
        }
}
static void Win32ResizeDIBSection(Win32_OffScreen_Buffer *Buffer, int Width, int Height)
{
    
    if(Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->BytesPerPixel = 4;

    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;

    int BitmapMemorySize = (Buffer->Width*Buffer->Height) * Buffer->BytesPerPixel;                                            
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
    Buffer->Pitch = Width*Buffer->BytesPerPixel;
}


static void Win32DisplayBuffer(HDC DeviceContext, int WindowWidth, int WindowHeight, Win32_OffScreen_Buffer Buffer, int Width, int Height)
{

    StretchDIBits(DeviceContext, 0, 0, WindowWidth, WindowHeight,
                                 0, 0, Buffer.Height, Buffer.Height,
                                 Buffer.Memory,
                                 &Buffer.Info,
                                 DIB_RGB_COLORS,
                                 SRCCOPY);
}
// Main Window Callback
LRESULT CALLBACK MainWindowCallBack(HWND Window,
                                    UINT Message,
                                    WPARAM WParam,
                                    LPARAM LParam)
{
    LRESULT Result = 0;
    switch(Message)
    {

        case WM_SIZE:
        {
        } break;

        case WM_DESTROY:
        {
            Running = false;
        } break;

        case WM_CLOSE:
        {   
            Running = false;
        } break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP");
        } break;

        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            Win32_Window_Dimension Dimension;
            HDC DeviceContext =  BeginPaint(Window, &Paint);
            
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;

            Win32DisplayBuffer(DeviceContext, Dimension.Width, Dimension.Height, GlobalBackBuffer, Width, Height);
            EndPaint(Window, &Paint);
        } break;

        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam); 
        } break;
    }

    return(Result);
}

// Windows entry-point function
int CALLBACK WinMain(HINSTANCE Instance, //  hInstance is the handle to an instance, OS uses this value to identify the .exe 
                     HINSTANCE PrevInstance, // No meaning
                     LPSTR CmdLine, // Command line arguments as unicode
                     int ShowCode) // Flag to indicate if window is minimized, maxed or shown normal
{
    WNDCLASS WindowClass = {0};

    Win32ResizeDIBSection(&GlobalBackBuffer, 1280, 720);

    
    WindowClass.style = CS_VREDRAW | CS_HREDRAW ; // Set of binary flags, basically a bitfield
    WindowClass.lpfnWndProc = MainWindowCallBack; // Pointer to a function that defines how our window responds to events
    WindowClass.hInstance = Instance; // Window instance 
    WindowClass.lpszClassName = "MyLiqClass"; // Window Class Name 

    if(RegisterClass(&WindowClass)) // Register class so that it can create a window for us,
    {
        HWND Window = 
        CreateWindowEx(0,
            WindowClass.lpszClassName,
            "MyLiqClass Window",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            Instance,
            0);

            if(Window) // Message que/loop, pull messages off the que
            {
                Running = true;
                

                int Xoffset = 0;
                int Yoffset = 0;
                while(Running)
                {
                    MSG Message;
                    while(PeekMessage(&Message, 0,  0, 0,  PM_REMOVE))
                    {
                        if(Message.message == WM_QUIT)
                        {
                            Running = false;
                        }
                        TranslateMessage(&Message);
                        DispatchMessage(&Message);
                    
                    }

                    

                    HDC DeviceContext = GetDC(Window);
                    Win32_Window_Dimension Dimension = Win32_GetWindowDimension(Window);

                    RenderGradient(GlobalBackBuffer, Xoffset, Yoffset);
                    Win32DisplayBuffer(DeviceContext, Dimension.Width, Dimension.Height, GlobalBackBuffer, Dimension.Width, Dimension.Height);
                    ReleaseDC(Window, DeviceContext);
                    ++Xoffset;
                }
            }
    }
    return(0);
}

