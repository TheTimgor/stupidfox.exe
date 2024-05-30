//////////////////////////////////////////////////////////
//////                                              //////
//////      AWFUL STUPID FUCKING FOX DESK PEST      //////
//////                                              //////
//////////////////////////////////////////////////////////

// no idea why i decided to write this as a win32 app but here u go
// in case you're wondering why it looks like shit its because i 
// scrapped it together from tutorials and code from my other projects 
// no i won't refactor it i'm too cute and sexy for that <3

#define UNICODE
#define _UNICODE
#include <windows.h>
#include <stdbool.h>
#include <stdint.h>
#include <wincon.h>
#include <stdio.h>
#include "pixeldata.h"

static bool quit = false;
static bool clicked = false;

static BITMAPINFO frame_bitmap_info;
static HBITMAP frame_bitmap = 0;
static HDC frame_device_context = 0;

#define IMWDTH 212
#define IMHGHT 219


struct {
    int width;
    int height;
    uint32_t *pixels;
} frame = {0};

LRESULT CALLBACK WindowProcessMessage(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow) {
    AttachConsole(ATTACH_PARENT_PROCESS);
    freopen("CON", "w", stdout);
    freopen("CON", "w", stderr);
    freopen("CON", "r", stdin);

    frame.width  = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    frame.height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    const wchar_t k_WndClassName[] = L"OverlayWindowClass";

    // Register window class
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof( wcex );
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WindowProcessMessage;
    wcex.hInstance      = hInstance;
    wcex.hCursor        = LoadCursorW( NULL, IDC_ARROW );
    // wcex.hbrBackground  = (HBRUSH)GetStockObject( BLACK_BRUSH );
    wcex.lpszClassName  = k_WndClassName;
    RegisterClassExW( &wcex );

    HWND window_handle = CreateWindowExW( WS_EX_TOPMOST | WS_EX_LAYERED,
                                   k_WndClassName,
                                   L"YEEP",
                                   WS_POPUP | WS_VISIBLE,
                                   CW_USEDEFAULT, CW_USEDEFAULT,
                                   frame.width, frame.height,
                                   NULL, NULL,
                                   hInstance,
                                   NULL );
    // Make window semi-transparent, and mask out background color
    SetLayeredWindowAttributes( window_handle, RGB( 0, 0, 0 ), 255, LWA_ALPHA | LWA_COLORKEY );


    frame_bitmap_info.bmiHeader.biSize = sizeof(frame_bitmap_info.bmiHeader);
    frame_bitmap_info.bmiHeader.biPlanes = 1;
    frame_bitmap_info.bmiHeader.biBitCount = 32;
    frame_bitmap_info.bmiHeader.biCompression = BI_RGB;
    frame_device_context = CreateCompatibleDC(0);

    frame_bitmap_info.bmiHeader.biWidth  = frame.width;
    frame_bitmap_info.bmiHeader.biHeight = frame.height;
    frame_bitmap = CreateDIBSection(NULL, &frame_bitmap_info, DIB_RGB_COLORS, (void**)&frame.pixels, 0, 0);
    SelectObject(frame_device_context, frame_bitmap);

    int direction = 1;
    int k = 0;
    int kclick = 0;
    int nextk = 200;
    int yshift = 0;
    int xshift = 0;
    while(!quit)
    {
        k++;
        static MSG message = { 0 };
        while(PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) { DispatchMessage(&message); }
        Sleep(50);
        if(k==nextk){
            k = 0;
            
            direction = -direction;
            nextk = 100 + (rand()%100);
            yshift = rand() % (frame.height-IMHGHT);

        }
        if(clicked){
            kclick++;
            if(kclick>=17){
                kclick = 0;
                clicked = false;
                k = 100;
            }
        }
        if(k>100){ continue; }

        if(window_handle == NULL) { return -1; }

        InvalidateRect(window_handle, NULL, FALSE);

        int offset = clicked ? 4*((kclick%17)+8)*IMWDTH*IMHGHT : 4*(k%8)*IMWDTH*IMHGHT;
        if(!clicked) { xshift = k*(frame.width+IMWDTH)/100 - IMWDTH; }
        for (int i = 0; i < frame.height; ++i){
            int ypos = (i-yshift);
            for (int j = 0; j < frame.width; ++j){
                int xpos = (direction > 0) ? (j-xshift) : frame.width - j - xshift;
                frame.pixels[i*frame.width + j] = 0;


                if((ypos<IMHGHT) && (ypos>0) && (xpos<IMWDTH) && (xpos>0)){                       
                    int pxidx = offset + 4*(ypos*IMWDTH + xpos);

                    if(data[pxidx + 3] < 10){
                        frame.pixels[i*frame.width + j] = 0; 
                    } else {
                        frame.pixels[i*frame.width + j] = 
                                (data[pxidx + 0] << (16)) + 
                                (data[pxidx + 1] << (8)) + 
                                (data[pxidx + 2]); 
                    }
                }
            }
        }
        UpdateWindow(window_handle);
    }

    return 0;
}


LRESULT CALLBACK WindowProcessMessage(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam) {
    switch(message) {
        case WM_QUIT:
        case WM_DESTROY: 
            quit = true;
        break;

        case WM_PAINT:
            static PAINTSTRUCT paint;
            static HDC device_context;
            device_context = BeginPaint(window_handle, &paint);
            BitBlt(device_context,
                   paint.rcPaint.left, paint.rcPaint.top,
                   paint.rcPaint.right - paint.rcPaint.left, paint.rcPaint.bottom - paint.rcPaint.top,
                   frame_device_context,
                   paint.rcPaint.left, paint.rcPaint.top,
                   SRCCOPY);
            EndPaint(window_handle, &paint);
        break;

        case WM_LBUTTONDOWN:
            clicked=true;

        default:
            return DefWindowProc(window_handle, message, wParam, lParam);
        
    }
    return 0;
}