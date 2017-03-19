#include "windows.h"
#include <stdio.h>

#define uint32 uint32_t
#define uint16 uint16_t
#define uint8 uint8_t

#define BYTES_PER_PIXEL 4
#define AUDIO_SAMPLE_PER_SEC 48000
#define BYTES_PER_SAMPLE (sizeof(INT16) * 2) 
#define AUDIO_BUFFER_SIZE (AUDIO_SAMPLE_PER_SEC * BYTES_PER_SAMPLE)
#define PI 3.14159265359

struct	GameOffScreenBuffer
{
	BITMAPINFO bitmapInfo;
	void *bitmapMemory;
	int bitmapWidth;
	int bitmapHeight;
};

void GameUpdateAndRender(GameOffScreenBuffer *gameBuffer, int xoffset, int yoffset);