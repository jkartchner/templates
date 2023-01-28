#include <windows.h>
#include <stdint.h>

#include "dbg.h"

#pragma comment (lib, "kernel32")
#pragma comment (lib, "user32")

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
		//case WM_PAINT:
		//	break;
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
		Sleep(50);
	}

	return;
}

// replacements for common funcs in CRT so we can function without CRT
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
