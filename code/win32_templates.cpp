#include <windows.h>
#include <stdint.h>

#include "dbg.h"

#define local_persist static 
#define global static 
#define internal static

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

#pragma comment (lib, "kernel32")
#pragma comment (lib, "user32")

internal void Resize_DIB_Section(HWND Window);
internal void win32_updatewindow(HWND Window);


struct Win32BitmapContainer 
{
	BITMAPINFO info;
	int bytes_per_pixel;
	void *memory;
} ;

global struct Win32BitmapContainer g_BitmapContainer;

LRESULT CALLBACK WindowProc(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
	LRESULT Result = 0;
	switch(Message)
	{
		case WM_CLOSE:
			DestroyWindow(Window);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_SIZE:
			Resize_DIB_Section(Window);
			break;
		case WM_PAINT:
			win32_updatewindow(Window);
			break;
		default:
			Result = DefWindowProc(Window, Message, wParam, lParam);
			break;
	}
	return (Result);
}

static HWND CreateOutputWindow()
{
	WNDCLASS WindowClass = {};
	{
		WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
		WindowClass.lpfnWndProc = &WindowProc;
		WindowClass.hInstance = GetModuleHandleW(NULL);
		WindowClass.hIcon = LoadIconA(NULL, IDI_APPLICATION);
		WindowClass.hCursor = LoadCursorA(NULL, IDC_ARROW);
		WindowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		WindowClass.lpszClassName = "templateclass";
	}

	HWND Result = {0};
	if(RegisterClass(&WindowClass))
	{
		Result = CreateWindowExA(0, WindowClass.lpszClassName, "Templates", WS_OVERLAPPEDWINDOW|WS_VISIBLE,
				         CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
					 0, 0, WindowClass.hInstance, 0);
	}

	return Result;
}

void  WinMainCRTStartup()
{
	HWND Window = CreateOutputWindow();
	Assert(IsWindow(Window));
	
	bool is_running = true;
	while(is_running)
	{
		MSG Message;

		while(PeekMessageA(&Message, 0, 0, 0, PM_REMOVE))
		{
			if(Message.message == WM_QUIT)
				is_running = false;
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		Sleep(100);
	}

	return;
}

internal void Resize_DIB_Section(HWND Window)
{
	if(g_BitmapContainer.memory)
	{
		VirtualFree(g_BitmapContainer.memory, 0, MEM_RELEASE);
	}

	RECT client_rect;
	GetClientRect(Window, &client_rect);
	int width = client_rect.right - client_rect.left;
	int height = client_rect.bottom - client_rect.top;

	{
		g_BitmapContainer.info.bmiHeader.biSize = sizeof(g_BitmapContainer.info.bmiHeader);
		g_BitmapContainer.info.bmiHeader.biWidth = width; 
		g_BitmapContainer.info.bmiHeader.biHeight = -height;  // we are forcing an inversion of the win32 API draw order here, since it goes bottom up
		g_BitmapContainer.info.bmiHeader.biPlanes = 1; 
		g_BitmapContainer.info.bmiHeader.biBitCount = 32; 
		g_BitmapContainer.info.bmiHeader.biCompression = BI_RGB; 
		g_BitmapContainer.info.bmiHeader.biSizeImage = 0; 
		g_BitmapContainer.info.bmiHeader.biXPelsPerMeter = 0; 
		g_BitmapContainer.info.bmiHeader.biYPelsPerMeter = 0; 
		g_BitmapContainer.info.bmiHeader.biClrUsed = 0; 
		g_BitmapContainer.info.bmiHeader.biClrImportant = 0; 
	}

	g_BitmapContainer.bytes_per_pixel = 4;
	
	int bitmap_memory_size = g_BitmapContainer.bytes_per_pixel * (width * height);

	g_BitmapContainer.memory = VirtualAlloc(0, bitmap_memory_size, MEM_COMMIT, PAGE_READWRITE);

}

internal void win32_renderbitmap(HWND Window)
{
	RECT client_rect;
	GetClientRect(Window, &client_rect);
	int width = client_rect.right - client_rect.left;
	int height = client_rect.bottom - client_rect.top;

	int pitch = width * g_BitmapContainer.bytes_per_pixel;
	u8 *row = (u8 *)g_BitmapContainer.memory;
	for(int y = 0; y < height; ++y)
	{
		u32 *pixel = (u32 *)row;
		for(int x = 0; x < width; ++x)
		{
			int blue = (x + y) * 25 / width; //50 - ((x + y) * 25 / width); 
			int green = 0;
			int red = 0;//(x + y) * 25 / width;

			*pixel++ = ((255 - blue - green << 16) | (255 - red - blue << 8) | (255 - red - green));  //A R G B
			
		}
		row += pitch;
	}
}

internal void win32_updatewindow(HWND Window) 
{
	win32_renderbitmap(Window);

	// BEGIN WINDOWS BLIT
	

	PAINTSTRUCT Paint;
	HDC dc = BeginPaint(Window, &Paint);

	int x = Paint.rcPaint.left;
	int y = Paint.rcPaint.top;
	int width = Paint.rcPaint.right - Paint.rcPaint.left;
	int height = Paint.rcPaint.bottom - Paint.rcPaint.top;

	// TODO: we are drawing only the "dirty" or messed up area of a window right now; 
	// but need to figure out a switch to switch to drawing blit the entire client area
	StretchDIBits(dc, x, y, width, height,  // destination
				      x, y, width, height, // source
				      g_BitmapContainer.memory, 
				      &g_BitmapContainer.info, 
				      DIB_RGB_COLORS, SRCCOPY);
	
	EndPaint(Window, &Paint);
}

// replacements for surreptitiously called funcs in CRT so we can function without CRT
int _fltused = 0x9875;

#pragma function(memset)
void *memset(void *DestInit, int Source, size_t Size)
{
	unsigned char *Dest = (unsigned char *)DestInit;
	while(Size--) *Dest++ = (unsigned char)Source;

	return (DestInit);
}

#pragma function(memcpy)
void *memcpy(void *DestInit, void const *SourceInit, size_t Size)
{
	unsigned char *Source = (unsigned char *)SourceInit;
	unsigned char *Dest = (unsigned char *)DestInit;
	while(Size--) *Dest++ = *Source++;

	return (DestInit);
}

