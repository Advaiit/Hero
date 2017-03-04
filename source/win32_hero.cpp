//Include the windows header
#include <Windows.h>
#include <stdint.h>

#define BYTES_PER_PIXEL 4
#define uint32 uint32_t
#define uint16 uint16_t
#define uint8 uint8_t

static bool gRunning = true;
static BITMAPINFO bitmapInfo;
static void *bitmapMemory;
static int bitmapWidth;
static int bitmapHeight;

static void renderGradient(int XOffset, int YOffset)
{
	int pitch = bitmapWidth * BYTES_PER_PIXEL;
	uint8 *row = (uint8 *)bitmapMemory;

	for (int Y = 0; Y < bitmapHeight; Y++)
	{
		uint8 *pixel = (uint8 *)row;

		for (int X = 0; X < bitmapWidth; X++)
		{
			*pixel = X + XOffset;
			pixel++;

			*pixel = Y + YOffset;
			pixel++;

			*pixel = 0;
			pixel++;

			*pixel = 0;
			pixel++;
		}

		row += pitch;
	}
}

//Devive Independent Bitmap
static void ResizeDIBSection(int width, int height)
{
	if (bitmapMemory)
	{
		VirtualFree(bitmapMemory, 0, MEM_RELEASE);
	}

	bitmapWidth = width;
	bitmapHeight = height;

	bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
	bitmapInfo.bmiHeader.biWidth = bitmapWidth;
	bitmapInfo.bmiHeader.biHeight = -bitmapHeight;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 32;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;

	bitmapMemory = VirtualAlloc(0, (bitmapWidth * bitmapHeight) * BYTES_PER_PIXEL, MEM_COMMIT, PAGE_READWRITE);

	return;
}

static void RefreshWindow(HDC deviceContext, RECT *windowRect, int x, int y, int width, int height)
{
	int windowWidth = windowRect->right - windowRect->left;
	int windowHeight = windowRect->bottom - windowRect->top;

	StretchDIBits(deviceContext,
		//x, y, width, height,
		//x, y, width, height,
		0, 0, bitmapWidth, bitmapHeight,
		0, 0, windowWidth, windowHeight,
		bitmapMemory,
		&bitmapInfo, 
		DIB_RGB_COLORS, 
		SRCCOPY);

	return;
}


LRESULT CALLBACK
MainWindowProc(HWND   window,
	UINT   message,
	WPARAM wParam,
	LPARAM lParam)
{
	LRESULT result = 0;

	switch (message)
	{
		case WM_SIZE:
		{
			RECT ClientRect;
			GetClientRect(window, &ClientRect);
			int width = ClientRect.right - ClientRect.left;
			int height = ClientRect.bottom - ClientRect.top;
			ResizeDIBSection(width, height);
			OutputDebugString("WM_SIZE\n");
		}break;
		case WM_QUIT:
		{
			OutputDebugStringA("WM_QUIT\n");
			gRunning = false;
		}break;
		case WM_DESTROY:
		{
			OutputDebugStringA("WM_DESTROY\n");
		}break;
		case WM_CLOSE:
		{
			OutputDebugStringA("WM_CLOSE\n");
			gRunning = false;
		}break;
		case WM_PAINT:
		{
			OutputDebugStringA("WM_PAINT\n");
			
			static bool flag = true;
			PAINTSTRUCT lpPaint = {};
			RECT ClientRect;
			GetClientRect(window, &ClientRect);

			HDC context = BeginPaint(window, &lpPaint);
			static DWORD operation = BLACKNESS;

			int X = lpPaint.rcPaint.left;
			int Y = lpPaint.rcPaint.top;
			int width = lpPaint.rcPaint.right - lpPaint.rcPaint.left;
			int height = lpPaint.rcPaint.bottom - lpPaint.rcPaint.top;

			RefreshWindow(context, &ClientRect, X, Y, width, height);

			//PatBlt(context, X, Y, width, height, operation);

			if (operation == BLACKNESS)
			{
				operation = WHITENESS;
			}
			else
			{
				operation = BLACKNESS;
			}
			EndPaint(window, &lpPaint);
		}break;
		default:
		{
			OutputDebugString("default\n");
			result = DefWindowProc(window, message, wParam, lParam);
		}
	}

	return result;

}

//Main entry point
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

	//Set up window class
	WNDCLASS wnd;
	wnd.cbClsExtra = 0;
	wnd.cbWndExtra = 0;
	wnd.hCursor = LoadCursor(0, IDC_ARROW);
	wnd.hIcon = LoadIcon(0, IDI_WINLOGO);
	wnd.lpszMenuName = 0;
	wnd.style = 0;
	wnd.hbrBackground = 0;
	wnd.lpfnWndProc = MainWindowProc;
	wnd.hInstance = hInstance;
	wnd.lpszClassName = "HandMadeHeroWindowClass";

	//Register window class
	if (RegisterClass(&wnd))
	{
		//Create window
		//! This returns NULL
		HWND hWnd = CreateWindowEx(
			0,
			wnd.lpszClassName,
			"Handmade Hero",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			NULL,
			NULL,
			hInstance,
			NULL
		);

		//Simple check to see if window creation failed
		if (hWnd == NULL) {
			//Pause
			system("PAUSE");
			return -1;
		}

		//Show the window
		ShowWindow(hWnd, nCmdShow);

		//Main message loop
		MSG msg;
		int XOffset = 0;
		int YOffset = 0;

		while (gRunning) {
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if(msg.message == WM_QUIT)
				{
					gRunning = false;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			renderGradient(XOffset, YOffset);

			RECT ClientRect;
			GetClientRect(hWnd, &ClientRect);

			HDC context = GetDC(hWnd);

			RefreshWindow(context, &ClientRect, 0, 0, bitmapWidth, bitmapHeight);
			
			ReleaseDC(hWnd, context);

			XOffset++;
		}
	}

	return S_OK;
}