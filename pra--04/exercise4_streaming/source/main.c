/*
The idea is to reproduce large information little by little in the sound buffer
*/

#define SAMPLERATE 22050 // Nyquist Frequence (22050Hz Samplerates per second)
#define SAMPLESPERBUF (SAMPLERATE / 30)
#define BYTESPERSAMPLE 4
#define START_NOTE_FREQUENCY 20	 // Hz
#define MAX_NOTE_FREQUENCY 20000 // Hz
#define STEP_FREQUENCY 100		 // Hz

//----------------------------------------------------------------------------
void fill_buffer(void *audioBuffer, size_t offset, size_t size, int frequency)
{
	//----------------------------------------------------------------------------

	u32 *dest = (u32 *)audioBuffer;

	for (int i = 0; i < size; i++)
	{
		// Sineidal Function
		s16 sample = INT16_MAX * sin(frequency * (2 * M_PI) * (offset + i) / SAMPLERATE);

		dest[i] = (sample << 16) | (sample & 0xffff);
	}

	DSP_FlushDataCache(audioBuffer, size);
}

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
	ndspWaveBuf waveBuf[2];

	// Pointer for Linear Allocation
	u32 *audioBuffer = (u32 *)linearAlloc(SAMPLESPERBUF * BYTESPERSAMPLE * 2);
	bool fillBlock = false;

	// Initializes NDSP.
	ndspInit();

	// Output mode to set. Defaults to NDSP_OUTPUT_STEREO. (STEREO: Left and right channel)
	ndspSetOutputMode(NDSP_OUTPUT_STEREO);

	// Sets the interpolation type of a channel.
	ndspChnSetInterp(0, NDSP_INTERP_LINEAR);

	// Sets the sample rate of a channel. ID of the channel (0..23).
	ndspChnSetRate(0, SAMPLERATE);

	/*
	Sets the format of a channel. ID of the channel (0..23). 
	PCM16. Pulse-code modulation(PCM), bit depth is the number of bits of information in each sample 
	and it directly corresponds to the resolution of each sample. 16 bits per sample
	*/
	ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);

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

	// Note Frequency

	// Fill with "0" all waveBuf elements and then then use the 0 and 1 channel
	memset(waveBuf, 0, sizeof(waveBuf));
	waveBuf[0].data_vaddr = &audioBuffer[0];			 // Data virtual address for channel 0 || Left
	waveBuf[0].nsamples = SAMPLESPERBUF;				 // Total number of samples for channel 0 || Left
	waveBuf[1].data_vaddr = &audioBuffer[SAMPLESPERBUF]; // Data virtual address for channel 1 || Right
	waveBuf[1].nsamples = SAMPLESPERBUF;				 // Total number of samples for channel 1 || Right

	// Stream Pointer
	size_t stream_offset = 0;

	// Current Note frequency
	size_t current_note_frequency = START_NOTE_FREQUENCY;

	fill_buffer(audioBuffer, stream_offset, SAMPLESPERBUF * 2, current_note_frequency);

	stream_offset += SAMPLESPERBUF;

	ndspChnWaveBufAdd(0, &waveBuf[0]);
	ndspChnWaveBufAdd(0, &waveBuf[1]);

	printf("Press up/down to change tone\n");

	while (aptMainLoop())
	{

		gfxSwapBuffers();
		gfxFlushBuffers();
		gspWaitForVBlank();

		hidScanInput();
		u32 kDown = hidKeysDown();

		consoleClear(); //Clear console

		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		if (kDown & KEY_DOWN)
		{
			current_note_frequency -= STEP_FREQUENCY;

			if (current_note_frequency < START_NOTE_FREQUENCY)
			{
				current_note_frequency = START_NOTE_FREQUENCY;
			}
		}

		if (kDown & KEY_UP)
		{
			current_note_frequency += STEP_FREQUENCY;

			if (current_note_frequency > MAX_NOTE_FREQUENCY)
				current_note_frequency = START_NOTE_FREQUENCY;
		}

		printf("\x1b[4;1HTONE GENERATOR");
		printf("\x1b[5;1HFor change the interval frequency in 100Hz steps");
		printf("\x1b[6;1HPress Up or Down");
		printf("\x1b[7;1HNote: %d Hz of %d Hz", current_note_frequency, MAX_NOTE_FREQUENCY);

		// Si sea ha ido reproduciendo la información suministrada, continua con la siguiente
		if (waveBuf[fillBlock].status == NDSP_WBUF_DONE)
		{
			fill_buffer(waveBuf[fillBlock].data_pcm16, stream_offset, waveBuf[fillBlock].nsamples, current_note_frequency);

			ndspChnWaveBufAdd(0, &waveBuf[fillBlock]);
			stream_offset += waveBuf[fillBlock].nsamples;

			fillBlock = !fillBlock;
		}
	}

	ndspExit();

	linearFree(audioBuffer);

	//Desinitializes the LCD framebuffers
	gfxExit();
	return 0;
}