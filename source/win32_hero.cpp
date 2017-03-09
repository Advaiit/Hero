//Include the windows header
#include <Windows.h>
#include <stdint.h>
#include <xinput.h>
#include <dsound.h>

#define BYTES_PER_PIXEL 4
#define AUDIO_SAMPLE_PER_SEC 48000
#define AUDIO_BUFFER_SIZE 48000 * sizeof(INT16) * 2

#define uint32 uint32_t
#define uint16 uint16_t
#define uint8 uint8_t

static LPDIRECTSOUNDBUFFER secondaryBuffer;

typedef DWORD xinput_get_state(DWORD dwUserIndex, XINPUT_STATE *pState);
DWORD XInputGetStateStub(DWORD dwUserIndex, XINPUT_STATE *pState)
{
	return ERROR_DEVICE_NOT_CONNECTED;
}

typedef DWORD xinput_set_state(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration);
DWORD XInputSetStateStub(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
{
	return ERROR_DEVICE_NOT_CONNECTED;
}

typedef HRESULT WINAPI direct_sound_create(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter);

static xinput_get_state *XInputGetState_ = XInputGetStateStub;
static xinput_set_state *XInputSetState_ = XInputSetStateStub;

#define XInputGetState XInputGetState_
#define XInputSetState XInputSetState_

static void LoadXInput()
{
	HMODULE XInputLibrary = LoadLibrary("xinput1_3.dll");

	if (XInputLibrary)
	{
		XInputGetState = (xinput_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
		XInputSetState = (xinput_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
	}
}

static void InitSound(HWND Window)
{
	HMODULE DSoundLibrary = LoadLibrary("dsound.dll");

	if (DSoundLibrary)
	{
		direct_sound_create *DirectSoundCreate = (direct_sound_create *)GetProcAddress(DSoundLibrary, "DirectSoundCreate");
		LPDIRECTSOUND pDirectSound;
		WAVEFORMATEX waveFormat = {};

		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = AUDIO_SAMPLE_PER_SEC;
		waveFormat.wBitsPerSample = 16;
		waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
		waveFormat.cbSize = 0;

		if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &pDirectSound, 0)))
		{
			if (SUCCEEDED(pDirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY)))
			{
				DSBUFFERDESC primaryBufferDescription = {};
				LPDIRECTSOUNDBUFFER primaryBuffer;

				primaryBufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
				primaryBufferDescription.dwSize = sizeof(primaryBufferDescription);

				if (SUCCEEDED(pDirectSound->CreateSoundBuffer(&primaryBufferDescription, &primaryBuffer, 0)))
				{
					if (SUCCEEDED(primaryBuffer->SetFormat(&waveFormat)))
					{
						OutputDebugString("Format Set");
					}
					else
					{

					}
				}
				else
				{

				}
			}
			else
			{

			}
		}
		else
		{

		}

		DSBUFFERDESC bufferDescription = {};
		bufferDescription.dwSize = sizeof(bufferDescription);
		bufferDescription.dwFlags = 0;
		bufferDescription.dwBufferBytes = AUDIO_BUFFER_SIZE;
		bufferDescription.lpwfxFormat = &waveFormat;

		if (SUCCEEDED(pDirectSound->CreateSoundBuffer(&bufferDescription, &secondaryBuffer, NULL)))
		{

		}
	}
}


struct	OffScreenBuffer
{
	BITMAPINFO bitmapInfo;
	void *bitmapMemory;
	int bitmapWidth;
	int bitmapHeight;
};

static OffScreenBuffer gOffBuffer;
static bool gRunning = true;

static void renderGradient(OffScreenBuffer *buffer, int XOffset, int YOffset)
{
	int pitch = buffer->bitmapWidth * BYTES_PER_PIXEL;
	uint8 *row = (uint8 *)buffer->bitmapMemory;

	for (int Y = 0; Y < buffer->bitmapHeight; Y++)
	{
		uint32 *pixel = (uint32 *)row;

		for (int X = 0; X < buffer->bitmapWidth; X++)
		{
			uint8 blue = X + XOffset;
			uint8 green = Y + YOffset;

			*pixel++ = green << 8 | blue;
		}

		row += pitch;
	}
}

//Devive Independent Bitmap
static void ResizeDIBSection(OffScreenBuffer *buffer, int width, int height)
{
	if (buffer->bitmapMemory)
	{
		VirtualFree(buffer->bitmapMemory, 0, MEM_RELEASE);
	}

	buffer->bitmapWidth = width;
	buffer->bitmapHeight = height;

	buffer->bitmapInfo.bmiHeader.biSize = sizeof(buffer->bitmapInfo.bmiHeader);
	buffer->bitmapInfo.bmiHeader.biWidth = buffer->bitmapWidth;
	buffer->bitmapInfo.bmiHeader.biHeight = -(buffer->bitmapHeight);
	buffer->bitmapInfo.bmiHeader.biPlanes = 1;
	buffer->bitmapInfo.bmiHeader.biBitCount = 32;
	buffer->bitmapInfo.bmiHeader.biCompression = BI_RGB;

	buffer->bitmapMemory = VirtualAlloc(0, (buffer->bitmapWidth * buffer->bitmapHeight) * BYTES_PER_PIXEL, MEM_COMMIT, PAGE_READWRITE);

	return;
}

static void RefreshWindow(HDC deviceContext, RECT *windowRect, OffScreenBuffer *buffer, int x, int y, int width, int height)
{
	int windowWidth = windowRect->right - windowRect->left;
	int windowHeight = windowRect->bottom - windowRect->top;

	StretchDIBits(deviceContext,
		0, 0, windowWidth, windowHeight,
		0, 0, buffer->bitmapWidth, buffer->bitmapHeight,
		buffer->bitmapMemory,
		&(buffer->bitmapInfo),
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
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			uint32 VKCode = wParam;

			bool wasDown = ((lParam & (1 << 30)) != 0);
			bool isDown = ((lParam & (1 << 31)) == 0);

			if (wasDown != isDown)
			{
				if (VKCode == 'W')
				{

				}
				else if (VKCode == 'A')
				{

				}
				else if (VKCode == 'S')
				{

				}
				else if (VKCode == 'D')
				{

				}
				else if (VKCode == 'Q')
				{

				}
				else if (VKCode == 'E')
				{

				}
				else if (VKCode == VK_UP)
				{

				}
				else if (VKCode == VK_LEFT)
				{

				}
				else if (VKCode == VK_RIGHT)
				{

				}
				else if (VKCode == VK_ESCAPE)
				{
					OutputDebugString("ESCAPE: ");

					if (isDown)
					{
						OutputDebugString("isDown ");
					}

					if (wasDown)
					{
						OutputDebugString("wasDown");
					}

					OutputDebugString("\n");

				}
				else if (VKCode == VK_SPACE)
				{

				}
			}
			
			if (VKCode == VK_F4 && (lParam & (1 << 29)))
			{
				gRunning = false;
			}
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

			RefreshWindow(context, &ClientRect, &gOffBuffer, X, Y, width, height);

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

	LoadXInput();

	ResizeDIBSection(&gOffBuffer, 1280, 720);

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

		InitSound(hWnd);

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

			for (int i = 0; i < XUSER_MAX_COUNT; i++)
			{
				XINPUT_STATE controllerState;

				if (XInputGetState(i, &controllerState) == ERROR_SUCCESS)
				{
					//The controller state is plugged in

					XINPUT_GAMEPAD *gamepad = &(controllerState.Gamepad);

					bool up = (gamepad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
					bool down = (gamepad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
					bool left = (gamepad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
					bool right = (gamepad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
					bool start = (gamepad->wButtons & XINPUT_GAMEPAD_START);
					bool back = (gamepad->wButtons & XINPUT_GAMEPAD_BACK);
					bool leftShoulder = (gamepad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
					bool rightShoulder = (gamepad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
					bool AButton = (gamepad->wButtons & XINPUT_GAMEPAD_A);
					bool BButton = (gamepad->wButtons & XINPUT_GAMEPAD_B);
					bool XButton = (gamepad->wButtons & XINPUT_GAMEPAD_X);
					bool YButton = (gamepad->wButtons & XINPUT_GAMEPAD_Y);

					INT16 StickX = gamepad->sThumbLX;
					INT16 StickY = gamepad->sThumbLY;

					XOffset += StickX >> 12;
					YOffset += StickY >> 12;
				}
				else
				{
					//The controller is not available
				}
			}

			renderGradient(&gOffBuffer, XOffset, YOffset);

			RECT ClientRect;
			GetClientRect(hWnd, &ClientRect);

			HDC context = GetDC(hWnd);

			RefreshWindow(context, &ClientRect, &gOffBuffer, 0, 0, gOffBuffer.bitmapWidth, gOffBuffer.bitmapHeight);
			
			ReleaseDC(hWnd, context);

			XOffset++;
		}
	}

	return S_OK;
}