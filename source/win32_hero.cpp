//Include the windows header
#include <Windows.h>

static bool gRunning = true;
static BITMAPINFO bitmapInfo;
static void *bitmapMemory;
static HBITMAP bitmapHandle;
static HDC deviceContext;

//Devive Independent Bitmap
static void ResizeDIBSection(int width, int height)
{
	if (bitmapHandle)
	{
		DeleteObject(bitmapHandle); 
	}

	if (!deviceContext)
	{
		deviceContext = CreateCompatibleDC(0);
	}

	bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
	bitmapInfo.bmiHeader.biWidth = width;
	bitmapInfo.bmiHeader.biHeight = height;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 32;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;

	bitmapHandle = CreateDIBSection(
		deviceContext,
		&bitmapInfo,
		DIB_RGB_COLORS,
		&bitmapMemory,
		0,
		0
	);

	return;
}

static void RefreshWindow(HDC deviceContext, int x, int y, int width, int height)
{
	StretchDIBits(deviceContext,
		x, y, width, height,
		x, y, width, height,
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

			HDC context = BeginPaint(window, &lpPaint);
			static DWORD operation = BLACKNESS;

			int X = lpPaint.rcPaint.left;
			int Y = lpPaint.rcPaint.top;
			int width = lpPaint.rcPaint.right - lpPaint.rcPaint.left;
			int height = lpPaint.rcPaint.bottom - lpPaint.rcPaint.top;

			RefreshWindow(context, X, Y, width, height);

			PatBlt(context, X, Y, width, height, operation);

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
		while (gRunning) {
			DWORD result = GetMessage(&msg, NULL, 0, 0);
			if (result)
			{

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
}