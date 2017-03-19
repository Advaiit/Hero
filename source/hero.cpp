#include "./hero.h"

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

void GameUpdateAndRender(GameOffScreenBuffer *gameBuffer, int xoffset, int yoffset)
{
	renderGradient(gameBuffer, xoffset, yoffset);
}