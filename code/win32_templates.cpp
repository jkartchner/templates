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


struct Win32BackBuffer
{
	bool is_init;
	BITMAPINFO info;
	int bytes_per_pixel;
	bool needs_blitting;
	void *memory;
} ;

struct NormalRect
{
	int x;
	int y;
	int width;
	int height;
}; 

internal void Resize_DIB_Section(HWND Window);
internal void win32_DrawtoBackBuffer(HWND Window);
internal void win32_BlittoScreen(HWND Window, HDC dc, int x, int y, int width, int height); 
internal NormalRect win32_GetClientDimensions(HWND Window);

global struct Win32BackBuffer g_BackBuffer;

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
			{
			PAINTSTRUCT Paint;
			HDC dc = BeginPaint(Window, &Paint);
			win32_BlittoScreen(Window, dc, Paint.rcPaint.left, 
						       Paint.rcPaint.top, 
						       Paint.rcPaint.right - Paint.rcPaint.left, 
						       Paint.rcPaint.bottom - Paint.rcPaint.top);
			EndPaint(Window, &Paint);
			} break;
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
		win32_DrawtoBackBuffer(Window);
		if(g_BackBuffer.needs_blitting)
		{
			HDC dc = GetDC(Window);
			NormalRect normal_rect = win32_GetClientDimensions(Window);
			win32_BlittoScreen(Window, dc, normal_rect.x, 
						       normal_rect.y, 
						       normal_rect.width, 
						       normal_rect.height); 
			g_BackBuffer.needs_blitting = false;
		}

		Sleep(100);
	}

	return;
}

internal void Resize_DIB_Section(HWND Window)
{
	if(g_BackBuffer.memory)
		VirtualFree(g_BackBuffer.memory, 0, MEM_RELEASE);

	NormalRect normal_rect = win32_GetClientDimensions(Window);
	int width = normal_rect.width;
	int height = normal_rect.height;

	{
		g_BackBuffer.info.bmiHeader.biSize = sizeof(g_BackBuffer.info.bmiHeader);
		g_BackBuffer.info.bmiHeader.biWidth = width; 
		g_BackBuffer.info.bmiHeader.biHeight = -height;  // we are forcing an inversion of the win32 API draw order here, since it goes bottom up
		g_BackBuffer.info.bmiHeader.biPlanes = 1; 
		g_BackBuffer.info.bmiHeader.biBitCount = 32; 
		g_BackBuffer.info.bmiHeader.biCompression = BI_RGB; 
		g_BackBuffer.info.bmiHeader.biSizeImage = 0; 
		g_BackBuffer.info.bmiHeader.biXPelsPerMeter = 0; 
		g_BackBuffer.info.bmiHeader.biYPelsPerMeter = 0; 
		g_BackBuffer.info.bmiHeader.biClrUsed = 0; 
		g_BackBuffer.info.bmiHeader.biClrImportant = 0; 
	}

	g_BackBuffer.bytes_per_pixel = 4;
	int bitmap_memory_size = g_BackBuffer.bytes_per_pixel * (width * height);

	g_BackBuffer.memory = VirtualAlloc(0, bitmap_memory_size, MEM_COMMIT, PAGE_READWRITE);
	if(g_BackBuffer.memory)
	{
		// TODO: The blit does not occur while resizing, despite a redraw
		// To solve this problem, we're drawing to the backbuffer here, but
		// this still causes flicker. Double buffer may fix
		g_BackBuffer.is_init = true;
		win32_DrawtoBackBuffer(Window);
		g_BackBuffer.needs_blitting = true;
	}

}

internal void win32_DrawtoBackBuffer(HWND Window)
{
	if(!g_BackBuffer.memory || !g_BackBuffer.needs_blitting)
		return;

	NormalRect normal_rect = win32_GetClientDimensions(Window);
	int width = normal_rect.width;
	int height = normal_rect.height;

	int pitch = width * g_BackBuffer.bytes_per_pixel;
	u8 *row = (u8 *)g_BackBuffer.memory;
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

internal void win32_BlittoScreen(HWND Window, HDC dc, int x, int y, int width, int height) 
{

	// BEGIN WINDOWS BLIT

	int tmp = StretchDIBits(dc, x, y, width, height,  // destination
				      x, y, width, height, // source
				      g_BackBuffer.memory, 
				      &g_BackBuffer.info, 
				      DIB_RGB_COLORS, SRCCOPY);
	
}

internal NormalRect win32_GetClientDimensions(HWND Window)
{
	
	RECT client_rect;
	NormalRect normal_rect;
	GetClientRect(Window, &client_rect);

	normal_rect.width = client_rect.right - client_rect.left;
	normal_rect.height = client_rect.bottom - client_rect.top;
	normal_rect.x = client_rect.left;
	normal_rect.y = client_rect.top;

	return normal_rect;

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

