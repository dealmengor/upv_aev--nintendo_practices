#include "main.h"

// Buffer Settings
#define BUFFER_TO_STREAM mostresMisterioses

// Audio Settings
#define SAMPLERATE 11025					   // Samplerates per second Hz
#define SAMPLESPERBUF sizeof(BUFFER_TO_STREAM) // Hz sampling rate
#define BYTESPERSAMPLE 1					   // 1 bytes per sample
int sound_status_sentinel = STAND_BY_STATE;

//----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	//----------------------------------------------------------------------------

	// Initializes the LCD framebuffers with default parameters
	gfxInitDefault();

	// Console Screen
	PrintConsole topScreen;
	consoleInit(GFX_TOP, &topScreen);
	consoleSelect(&topScreen);

	// Wave buffer struct and type
	ndspWaveBuf waveBuf[1];

	// Pointer for Linear Allocation
	u32 *audioBuffer = (u32 *)linearAlloc(SAMPLESPERBUF * BYTESPERSAMPLE * 2);

	// Initializes NDSP.
	ndspInit();

	// Output mode to set.
	ndspSetOutputMode(NDSP_OUTPUT_MONO);

	// Sets the interpolation type of a channel.
	ndspChnSetInterp(0, NDSP_INTERP_LINEAR);

	// Sets the sample rate of a channel. ID of the channel (0..23).
	ndspChnSetRate(0, SAMPLERATE);

	/*
	Sets the format of a channel. ID of the channel (0..23). 
	PCM16. Pulse-code modulation(PCM), bit depth is the number of bits of information in each sample 
	and it directly corresponds to the resolution of each sample. 16 bits per sample
	*/
	ndspChnSetFormat(0, NDSP_FORMAT_PCM8);

	/*
	Sets the mix parameters (volumes) of a channel.
	0: Front left volume.
	1: Front right volume.
	2: Back left volume:
	3: Back right volume:
	4..7: Same as 0..3, but for auxiliary output 0.
	8..11: Same as 0..3, but for auxiliary output 1.
	*/
	float mix[12];
	memset(mix, 0, sizeof(mix)); // Fill with "0" all mix elements and then use the 0 and 1 channel
	mix[0] = 1.0;
	mix[1] = 1.0;
	ndspChnSetMix(0, mix);

	// Fill with "0" all waveBuf elements and then then use the 0 and 1 channel
	memset(waveBuf, 0, sizeof(waveBuf));
	waveBuf[0].data_vaddr = memcpy(&audioBuffer[0], BUFFER_TO_STREAM, sizeof(BUFFER_TO_STREAM)); // Data virtual address
	waveBuf[0].nsamples = SAMPLESPERBUF;														 // Total number of samples

	ndspChnWaveBufAdd(0, &waveBuf[0]);

	// Console
	printf("\x1b[4;1HMostres Misterioses");

	while (aptMainLoop())
	{

		gfxSwapBuffers();
		gfxFlushBuffers();
		gspWaitForVBlank();

		hidScanInput();
		u32 kDown = hidKeysDown();

		// Program Input
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu
		}

	ndspExit();

	linearFree(audioBuffer);

	//Desinitializes the LCD framebuffers
	gfxExit();
	return 0;
}