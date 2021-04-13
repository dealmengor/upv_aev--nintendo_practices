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

#define MAX_SPRITES 3
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SPEED 1

// Simple sprite struct
typedef struct
{
	C2D_Sprite spr;
	float dx, dy; // velocity
} Sprite;

static C2D_SpriteSheet spriteSheet;
static Sprite sprites[MAX_SPRITES];
static size_t numSprites = MAX_SPRITES;

static void initSprites()
{

	srand(time(NULL)); // Sets a seed for random numbers

	for (size_t i = 0; i < MAX_SPRITES; i++)
	{
		Sprite *sprite = &sprites[i];
		// Random image, position, rotation and SPEED
		C2D_SpriteFromSheet(&sprite->spr, spriteSheet, i);
		C2D_SpriteSetCenter(&sprite->spr, 0.5f, 0.5f);
		C2D_SpriteSetPos(&sprite->spr, rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
		C2D_SpriteSetRotation(&sprite->spr, C3D_Angle(rand() / (float)RAND_MAX));
		sprite->dx = rand() * 4.0f / RAND_MAX - 2.0f;
		sprite->dy = rand() * 4.0f / RAND_MAX - 2.0f;
		if (i == 2)
		{
			C2D_SpriteSetPos(&sprite->spr, 100, 100);
			C2D_SpriteSetRotationDegrees(&sprite->spr, 180);
		}
	}
}

static void moveSprites()
{

	for (size_t i = 0; i < numSprites - 1; i++)
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

static void movePlayerSprite()
{
	Sprite *sprite = &sprites[2];
	//LEFT BORDER
	if (sprite->spr.params.pos.x < SCREEN_WIDTH && sprite->spr.params.pos.x > 0)
	{
		C2D_SpriteMove(&sprite->spr, sprite->dx, 0);
	}
	//RIGHT BORDER
	else
	{
		if (sprite->spr.params.pos.x >= SCREEN_WIDTH)
			C2D_SpriteMove(&sprite->spr, -SPEED, 0);
		else
			C2D_SpriteMove(&sprite->spr, SPEED, 0);
	}

	if (sprite->spr.params.pos.y <= SCREEN_HEIGHT && sprite->spr.params.pos.y > 0)
	{
		C2D_SpriteMove(&sprite->spr, 0, sprite->dy);
	}
	else
	{
		if (sprite->spr.params.pos.y > SCREEN_HEIGHT)
		{
			C2D_SpriteMove(&sprite->spr, 0, -SPEED);
		}
		else
			C2D_SpriteMove(&sprite->spr, 0, SPEED);
	}
	sprite->dx = 0;
	sprite->dy = 0;
}

static void movePlayerController(u32 kHeld)
{
	Sprite *sprite = &sprites[2];
	//UP
	if (kHeld & KEY_UP)
	{
		sprite->dy += -SPEED;
	}
	// //DOWN
	if (kHeld & KEY_DOWN)
	{
		sprite->dy += SPEED;
	}
	//LEFT
	if (kHeld & KEY_LEFT)
	{
		sprite->dx += -SPEED;
	}
	//RIGHT
	if (kHeld & KEY_RIGHT)
	{
		sprite->dx += SPEED;
	}
}

int main(int argc, char *argv[])
{

	// Init libs
	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	// Create screens
	C3D_RenderTarget *bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	// Load graphics
	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!spriteSheet)
		svcBreak(USERBREAK_PANIC);

	// Initialize sprites
	initSprites();

	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();

		// Respond to user input
		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();

		// Interface Control Logic
		// break in order to return to hbmenu
		if (kDown & KEY_START)
			break;

		// D-PAD Controller
		if (kHeld & KEY_UP || kHeld & KEY_DOWN || kHeld & KEY_LEFT || kHeld & KEY_RIGHT)
			movePlayerController(kHeld);

		// Move sprites
		moveSprites();
		movePlayerSprite();

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(bottom, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f));
		C2D_SceneBegin(bottom);
		for (size_t i = 0; i < numSprites; i++)
			C2D_DrawSprite(&sprites[i].spr);
		C3D_FrameEnd(0);
	}

	// Delete graphics
	C2D_SpriteSheetFree(spriteSheet);

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
	return 0;
}
