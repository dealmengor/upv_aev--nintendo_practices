// Simple citro2d sprite drawing example
// Images borrowed from:
//   https://kenney.nl/assets/space-shooter-redux
#include <citro2d.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <3ds.h>

#define MAX_SPRITES 2
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// Simple sprite struct
typedef struct
{
	C2D_Sprite spr;
	float dx, dy; // velocity
} Sprite;

static C2D_SpriteSheet spriteSheet;
static Sprite sprites[MAX_SPRITES];
static size_t numSprites = MAX_SPRITES;

C2D_TextBuf g_staticBuf, g_dynamicBuf;
C2D_Text g_staticText[4];

static void sceneInit(void)
{
	// Create two text buffers: one for static text, and another one for
	// dynamic text - the latter will be cleared at each frame.
	g_staticBuf = C2D_TextBufNew(4096); // support up to 4096 glyphs in the buffer
	g_dynamicBuf = C2D_TextBufNew(4096);

	// Parse the static text strings
	C2D_TextParse(&g_staticText[0], g_staticBuf, "Instructions");
	C2D_TextParse(&g_staticText[1], g_staticBuf, "Move up: ↑");
	C2D_TextParse(&g_staticText[2], g_staticBuf, "Move left: ←");
	C2D_TextParse(&g_staticText[3], g_staticBuf, "Move right: →");
	C2D_TextParse(&g_staticText[4], g_staticBuf, "Move down: ↓");

	// Optimize the static text strings
	C2D_TextOptimize(&g_staticText[0]);
	C2D_TextOptimize(&g_staticText[1]);
	C2D_TextOptimize(&g_staticText[2]);
	C2D_TextOptimize(&g_staticText[3]);
	C2D_TextOptimize(&g_staticText[4]);
}

static void sceneRender(float size)
{
	// Clear the dynamic text buffer
	C2D_TextBufClear(g_dynamicBuf);

	// Draw static text strings
	C2D_DrawText(&g_staticText[0], C2D_WithColor | C2D_AlignCenter, 60.0f, 25.0f, 0.5f, size, size, C2D_Color32f(0.0f, 0.0f, 1.0f, 0.350f));
	C2D_DrawText(&g_staticText[1], C2D_WithColor | C2D_AlignJustified, 10.0f, 40.0f, 0.5f, size, size, C2D_Color32f(0.0f, 0.0f, 1.0f, 0.625f));
	C2D_DrawText(&g_staticText[2], C2D_WithColor | C2D_AlignJustified, 10.0f, 55.0f, 0.5f, size, size, C2D_Color32f(0.0f, 0.0f, 1.0f, 0.625f));
	C2D_DrawText(&g_staticText[3], C2D_WithColor | C2D_AlignJustified, 10.0f, 70.0f, 0.5f, size, size, C2D_Color32f(0.0f, 0.0f, 1.0f, 0.625f));
	C2D_DrawText(&g_staticText[4], C2D_WithColor | C2D_AlignJustified, 10.0f, 85.0f, 0.5f, size, size, C2D_Color32f(0.0f, 0.0f, 1.0f, 0.625f));

	// Generate and draw dynamic text
	char buf[160];

	// C2D_Text dynText;
	// snprintf(buf, sizeof(buf), "Current text size: %f (Use  to change)", size);
	// C2D_TextParse(&dynText, g_dynamicBuf, buf);
	// C2D_TextOptimize(&dynText);
	// C2D_DrawText(&dynText, C2D_AlignCenter, 200.0f, 145.0f, 0.5f, 0.5f, 0.5f);

	C2D_Text Sprites;
	snprintf(buf, sizeof(buf), "Sprites:     %zu/%u", numSprites, MAX_SPRITES);
	C2D_TextParse(&Sprites, g_dynamicBuf, buf);
	C2D_TextOptimize(&Sprites);
	C2D_DrawText(&Sprites, C2D_AlignCenter | C2D_WordWrap | C2D_WithColor, 200.0f, 170.0f, 0.5f, 0.5f, 0.5f, C2D_Color32f(1.0f, 0.0f, 0.0f, 1.0f));

	C2D_Text CPU;
	snprintf(buf, sizeof(buf), "CPU:     %6.2f%%", C3D_GetProcessingTime() * 6.0f);
	C2D_TextParse(&CPU, g_dynamicBuf, buf);
	C2D_TextOptimize(&CPU);
	C2D_DrawText(&CPU, C2D_AlignCenter, 200.0f, 205.0f, 0.5f, 0.5f, 0.5f);

	C2D_Text GPU;
	snprintf(buf, sizeof(buf), "GPU:     %6.2f%%", C3D_GetDrawingTime() * 6.0f);
	C2D_TextParse(&GPU, g_dynamicBuf, buf);
	C2D_TextOptimize(&GPU);
	C2D_DrawText(&GPU, C2D_AlignCenter, 200.0f, 215.0f, 0.5f, 0.5f, 0.5f);

	// C2D_Text Cmd;
	// snprintf(buf, sizeof(buf), "CmdBuf:  %6.2f%%", C3D_GetCmdBufUsage() * 100.0f);
	// C2D_TextParse(&Cmd, g_dynamicBuf, buf);
	// C2D_TextOptimize(&Cmd);
	// C2D_DrawText(&Cmd, C2D_AlignCenter, 200.0f, 225.0f, 0.5f, 0.5f, 0.5f);
}

static void sceneExit(void)
{
	// Delete the text buffers
	C2D_TextBufDelete(g_dynamicBuf);
	C2D_TextBufDelete(g_staticBuf);
}

//---------------------------------------------------------------------------------
static void initSprites()
{
	//---------------------------------------------------------------------------------
	//size_t numImages = C2D_SpriteSheetCount(spriteSheet);
	srand(time(NULL)); // Sets a seed for random numbers

	for (size_t i = 0; i < MAX_SPRITES; i++)
	{
		Sprite *sprite = &sprites[i];

		// Random image, position, rotation and speed
		C2D_SpriteFromSheet(&sprite->spr, spriteSheet, i);
		C2D_SpriteSetCenter(&sprite->spr, 0.5f, 0.5f);
		C2D_SpriteSetPos(&sprite->spr, rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
		C2D_SpriteSetRotation(&sprite->spr, C3D_Angle(rand() / (float)RAND_MAX));
		sprite->dx = rand() * 4.0f / RAND_MAX - 2.0f;
		sprite->dy = rand() * 4.0f / RAND_MAX - 2.0f;
	}
}

//---------------------------------------------------------------------------------
static void moveSprites()
{
	//---------------------------------------------------------------------------------
	for (size_t i = 0; i < numSprites; i++)
	{
		Sprite *sprite = &sprites[i];
		C2D_SpriteMove(&sprite->spr, sprite->dx, sprite->dy);
		C2D_SpriteRotateDegrees(&sprite->spr, 1.0f);

		// Check for collision with the screen boundaries
		if ((sprite->spr.params.pos.x < sprite->spr.params.pos.w / 2.0f && sprite->dx < 0.0f) ||
			(sprite->spr.params.pos.x > (SCREEN_WIDTH - (sprite->spr.params.pos.w / 2.0f)) && sprite->dx > 0.0f))
			sprite->dx = -sprite->dx;

		if ((sprite->spr.params.pos.y < sprite->spr.params.pos.h / 2.0f && sprite->dy < 0.0f) ||
			(sprite->spr.params.pos.y > (SCREEN_HEIGHT - (sprite->spr.params.pos.h / 2.0f)) && sprite->dy > 0.0f))
			sprite->dy = -sprite->dy;
	}
}

//---------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	//---------------------------------------------------------------------------------
	// Init libs
	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	//Console Display
	// consoleInit(GFX_TOP, NULL);

	// Create screens
	C3D_RenderTarget *top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	C3D_RenderTarget *bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	// Load graphics
	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!spriteSheet)
		svcBreak(USERBREAK_PANIC);

	// Initialize sprites
	initSprites();

	// Initialize the scene
	sceneInit();

	float size = 0.5f;

	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();

		// Respond to user input
		// Respond to user input
		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		if (kHeld & KEY_L)
			size -= 1.0f / 128;
		else if (kHeld & KEY_R)
			size += 1.0f / 128;
		else if (kHeld & KEY_X)
			size = 0.5f;
		else if (kHeld & KEY_Y)
			size = 1.0f;

		if (size < 0.25f)
			size = 0.25f;
		else if (size > 2.0f)
			size = 2.0f;

		moveSprites();

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

		C2D_TargetClear(top, C2D_Color32(0x68, 0xB0, 0xD8, 0xFF));
		C2D_SceneBegin(top);
		sceneRender(size);

		C2D_TargetClear(bottom, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f));
		C2D_SceneBegin(bottom);

		for (size_t i = 0; i < numSprites; i++)
			C2D_DrawSprite(&sprites[i].spr);
		C3D_FrameEnd(0);
	}

	// Deinitialize the scene
	sceneExit();

	// Delete graphics
	C2D_SpriteSheetFree(spriteSheet);

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
	return 0;
}
