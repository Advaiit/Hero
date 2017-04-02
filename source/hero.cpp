#include "./hero.h"

static void GameOutputSound(GameOutputSoundBuffer *buffer)
{
	if (buffer)
	{
		INT16 *sampleOut = (INT16 *)buffer->samples;

		if (sampleOut)
		{
			for (int i = 0; i < buffer->sampleCount; i++)
			{
				float t = 2.0f * PI * buffer->soundData.runningSampleIndex++ / (float)buffer->soundData.WavePeriod;
				float sineValue = sinf(t);
				INT16 SampleValue = (INT16)(sineValue * buffer->soundData.volume);
				*sampleOut++ = SampleValue;
				*sampleOut++ = SampleValue;
			}
		}
		else
		{
			//TODO: Handle this
		}
	}
	else
	{
		//TODO: Handle this
	}
}

static void renderGradient(GameOffScreenBuffer *buffer, int XOffset, int YOffset)
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

void GameUpdateAndRender(GameOffScreenBuffer *gameBuffer, int xoffset, int yoffset, GameOutputSoundBuffer *soundBuffer)
{
	renderGradient(gameBuffer, xoffset, yoffset);
	GameOutputSound(soundBuffer);
}