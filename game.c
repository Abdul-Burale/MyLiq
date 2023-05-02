#include <windows.h>
#include <stdbool.h>
#include <stdint.h>

#define Locale_var static 
#define Global_var static 



Global_var bool Running;

Global_var void *BitmapMemory;
Global_var BITMAPINFO BitmapInfo;
Global_var int BitmapHeight;
Global_var int BitmapWidth; 

typedef struct Win32_OffScreen_Buffer {
        BITMAP Info;
        void *Memory;
        int Width;
        int Height;
        int Pitch;
        int BytesPerPixel;

} Win32_OffScreen_Buffer;
static void Win32ResizeDIBSection(int Width, int Height)
{
    
    if(BitmapMemory)
    {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }

    BitmapWidth = Width;
    BitmapHeight = Height;

    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = BitmapWidth;
    BitmapInfo.bmiHeader.biHeight = BitmapHeight;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;

    int BytesPerBytes = 4;
    int BitmapMemorySize = (Width*Height) * BytesPerBytes;                                            
    BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

    int Pitch = Width*BytesPerBytes;
    uint8_t *Row = (uint8_t*)BitmapMemory;
    for (int Y = 0;
        Y < BitmapHeight; Y++)
        {
            uint8_t *Pixel = (uint8_t*)Row; 
            for (int X = 0;
                X < BitmapWidth; X++)
                {
                    // Blue
                    *Pixel = 0;
                    ++Pixel;
                    
                    // Green
                    *Pixel = (uint8_t)X;
                    ++Pixel;

                    // Red
                    *Pixel = (uint8_t)Y;
                    ++Pixel;
                    
                    // Padding
                    *Pixel = 0;
                    ++Pixel;
                }

                Row += Pitch;
        }
}   

static void Win32UpdateWindow(HDC DeviceContext, RECT *WindowRect, int X, int Y, int Width, int Height)
{
    int WindowWidth = WindowRect->right - WindowRect->left;
    int WindowHeight = WindowRect->bottom - WindowRect->top;

    StretchDIBits(DeviceContext, 0, 0, BitmapWidth, BitmapHeight,
                                 X, Y, WindowWidth, WindowHeight,
                                 BitmapMemory,
                                 &BitmapInfo,
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
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            int Width = ClientRect.right - ClientRect.left;
            int Height = ClientRect.bottom - ClientRect.top;
            Win32ResizeDIBSection(Width, Height);
            OutputDebugStringA("WM_SIZE");
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
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            HDC DeviceContext =  BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
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
    
    WindowClass.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW ; // Set of binary flags, basically a bitfield
    WindowClass.lpfnWndProc = MainWindowCallBack; // Pointer to a function that defines how our window responds to events
    WindowClass.hInstance = Instance; // Window instance 
    WindowClass.lpszClassName = "MyLiqClass"; // Window Class Name 

    if(RegisterClass(&WindowClass)) // Register class so that it can create a window for us,
    {
        HWND WindowHandle = 
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

            if(WindowHandle) // Message que/loop, pull messages off the que
            {
                Running = true;
                MSG Message;
                while(Running)
                {
                    BOOL MessageResult = GetMessageA(&Message, 0, 0, 0);
                    if(MessageResult > 0)
                    {
                        TranslateMessage(&Message);
                        DispatchMessage(&Message);
                    }
                    else
                    {
                        break;
                    }
                }
            }
    }
    return(0);
}
