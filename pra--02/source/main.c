/*
	Based on the work of Circle Pad example made by Aurelio Mannara for libctru
	by Alexander Almengor
*/

#include <3ds.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv)
{
	// Initialize services
	gfxInitDefault();

	//Initialize console on top screen. Using NULL as the second argument tells the console library to use the internal console structure as current one
	consoleInit(GFX_TOP, NULL);

	//Click Counter and timer
	int counter = 0, timer = 3;

	// Timer
	time_t current_epoch_time, next_stop;
	time(&current_epoch_time);
	time(&next_stop);
	double diff_t = difftime(next_stop, current_epoch_time);

	// Main loop
	while (aptMainLoop())
	{
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();

		//hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
		u32 kDown = hidKeysDown();

		//Instrucctions
		printf("\x1b[5;9HCounter Clicks App 3DS");
		printf("\x1b[6;9HHow many clicks can you make in 3 seconds?");
		printf("\x1b[7;9HPress A to Start/Restart");
		printf("\x1b[8;9HPress Start to exit");

		//Timer
		time(&current_epoch_time);						  // Get current EPOCH time from System
		diff_t = difftime(next_stop, current_epoch_time); // Time Difference
		if (diff_t >= 0 && diff_t <= 3)					  // less than 3 seconds and more than 3 the timer
			timer = diff_t;
		printf("\x1b[9;9HTimer: %d", (int)timer);
		if (diff_t < 0) // if the difft is not lees than 3 secs and more than 3
			counter = 0;
		printf("\x1b[10;9HClicks: %d", (int)counter);
		//printf("\x1b[2;9HDiff = %f", (double)diff_t);

		// Keys validatation
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		if (kDown & KEY_A)
		{
			// Get Next_Stop for timer
			next_stop = current_epoch_time + 3;
			consoleClear(); //Clear console
		}

		// Touch Screen Validation
		// Do if the touch screen has been touched clicks +1
		if (kDown & KEY_TOUCH)
		{
			if (diff_t > 0 && diff_t <= 3)
				counter++;
		}

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();

		//Wait for VBlank
		gspWaitForVBlank();
	}

	// Exit services
	gfxExit();
	return 0;
}
