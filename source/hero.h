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

struct SoundData_
{
	int Hertz = 256;
	uint32 runningSampleIndex = 0;
	int WaveCounter = 0;
	int WavePeriod = AUDIO_SAMPLE_PER_SEC / Hertz;
	int halfWavePeriod = WavePeriod / 2;
	int volume = 4000;
	int soundPlaying = false;
};

struct GameOutputSoundBuffer
{
	SoundData_ soundData;
	INT16 *samples;
	int sampleCount;
};

void GameUpdateAndRender(GameOffScreenBuffer *gameBuffer, int xoffset, int yoffset, GameOutputSoundBuffer *soundBuffer);