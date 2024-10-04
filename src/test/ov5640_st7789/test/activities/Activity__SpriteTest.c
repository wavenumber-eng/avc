#include "eGFX.h"
#include "ASSETS/Sprites/Sprites_16BPP_RGB565.h"
#include "math.h"

int Cnt = 0;

#define NUM_SPRITES		3

static eGFX_Obj_Sprite Sprite[NUM_SPRITES];
static eGFX_Point_Animator PointAnim[NUM_SPRITES];
static eGFX_Scalar_Animator ScalarAnim[NUM_SPRITES];

#define FRAC_X	0.16
#define FRAC_Y  0.16
#define MAX_FRAMES	200
#define ANIM_MODE_POS eGFX_ANIMATOR_MODE__FRACTIONAL_BISECT
#define ANIM_MODE_RAD eGFX_ANIMATOR_MODE__LINEAR

static uint32_t AnimPhase = 0;

static void *ObjectList = &Sprite[0];

static void *AnimatorList = &PointAnim[0];

static uint32_t SpriteTestActiveBuffer = 0;

static eGFX_ImagePlane * BackgroundImage;

static eGFX_ImagePlane * ForegroundImage;

static eGFX_PixelState BackgroundColor = 0;

void Activity__Sprite_Enter(uint32_t MessageID, void *Message)
{

	BackgroundImage = (eGFX_ImagePlane *)(Message);
	BackgroundColor = MessageID;
	
	SpriteTestActiveBuffer = 0;

	ForegroundImage = &eGFX_BackBuffer[SpriteTestActiveBuffer];
	
	if (Message != NULL)
	{
		for (int i = 0; i < eGFX_NUM_BACKBUFFERS; i++)
		{
			eGFX_Blit(&eGFX_BackBuffer[i], 0, 0, BackgroundImage);
		}
	}
	else
	{
		eGFX_Rect R;

		R.P1.X = 0;
		R.P1.Y = 0;
		R.P2.X = eGFX_PHYSICAL_SCREEN_SIZE_X - 1;
		R.P2.Y = eGFX_PHYSICAL_SCREEN_SIZE_Y - 1;

		for (int i = 0; i < eGFX_NUM_BACKBUFFERS; i++)
		{
			eGFX_DrawSolidRectangle(&eGFX_BackBuffer[i], &R, BackgroundColor);
		}
	}
	
	for (int i = 0; i < NUM_SPRITES; i++)
	{
		eGFX_InitObject_Sprite(&Sprite[i], 0, &ForegroundImage);



		Sprite[i].TransparentColor = 0;
		Sprite[i].RenderOption = 1;

		if (eGFX_NUM_BACKBUFFERS == 2)
			Sprite[i].DoubleBuffered = true;
		else
			Sprite[i].DoubleBuffered = false;

		if (i != 0)
			eGFX_AddObject(ObjectList, &Sprite[i]);
			
		if (Message != NULL)
		{
			Sprite[i].Header.DirtyRectangeRestore = eGFX_DIRTY_RECTANGLE_RESTORE_BACKGROUND_IMAGE;


			Sprite[i].Header.BackgroundImage = &BackgroundImage;
		}
		else
		{

			Sprite[i].Header.DirtyRectangeRestore = eGFX_DIRTY_RECTANGLE_RESTORE_BACKGROUND_COLOR;
			Sprite[i].Header.BackgroundColor = &BackgroundColor;
		}

		Sprite[i].Position.X = -50*i;
		Sprite[i].Position.Y = 60 + i * 60;



		eGFX_Init_PointAnimator(&PointAnim[i],
			Sprite[i].Position,
			(eGFX_Point) {
			200, 60 + i * 60
		},
			&(Sprite[i].Position), //Point this to the thing you want to animate
				MAX_FRAMES,      //Maximum allowed frames before forced convergence
				(eGFX_PointF) {
				FRAC_X, FRAC_Y
			}, //The amount of percentage of the distance the point should move each step. Values <=0 or >= 1 are set to 0.5
				(eGFX_ObjectHeader *)&Sprite[i], // Object to Mark when animation is stepped;
					eGFX_ANIMATOR_MODE__FRACTIONAL_BISECT
					);

		if (i != 0)
			eGFX_AddAnimator(AnimatorList, &PointAnim[i]);

	}
	Sprite[0].Sprite = &Sprite_16BPP_RGB565_Menu1;
	Sprite[1].Sprite = &Sprite_16BPP_RGB565_Menu2;
	Sprite[2].Sprite = &Sprite_16BPP_RGB565_Menu3;

	eGFX_StartAnimators(AnimatorList);
	eGFX_Dump(&eGFX_BackBuffer[SpriteTestActiveBuffer]);
	switch (MessageID)
	{
		//System Messages...   Do nothing if you don't need the behavior
	case	eGFX_ACTIVITY_MSG_ID_RESET:
		break;

	case	eGFX_ACTIVITY_MSG_ID_NO_MSG:
		break;

	case	eGFX_ACTIVITY_MSG_ID_POP:
		break;

	case	eGFX_ACTIVITY_MSG_ID_PUSH:
		break;

	case	eGFX_ACTIVITY_MSG_ID_SWITCH:
		break;

	default:
		break;

	}
	return;
}


void Activity__Sprite_Process()
{
	
	if (eGFX_NUM_BACKBUFFERS == 2)
	{
		SpriteTestActiveBuffer++;

		SpriteTestActiveBuffer &= 0x01;
	}
	else
	{
		SpriteTestActiveBuffer = 0;
	}
	
	ForegroundImage = &eGFX_BackBuffer[SpriteTestActiveBuffer];
	
    eGFX_ProcessAnimators(AnimatorList);

	if (eGFX_AnimatorsAreComplete(AnimatorList) == eGFX_TRUE)
	{
		AnimPhase++;
		AnimPhase &= 0x1;
		for (int i = 0; i < NUM_SPRITES; i++)
		{
			int TargetX;
			int TargetY;

			if (AnimPhase)
			{
				 TargetX = -250 * (i+1) - 50;
				 TargetY = Sprite[i].Position.Y;
			}
			else
			{
				 TargetX = 240;
				 TargetY = Sprite[i].Position.Y;
			}
			eGFX_Init_PointAnimator(&PointAnim[i],
				Sprite[i].Position,
				(eGFX_Point) {
				TargetX, TargetY
			},
				&(Sprite[i].Position), //Point this to the thing you want to animate
					MAX_FRAMES,      //Maximum allowed frames before forced convergence
					(eGFX_PointF) {
					FRAC_X, FRAC_Y
				}, //The amount of percentage of the distance the point should move each step. Values <=0 or >= 1 are set to 0.5
					(eGFX_ObjectHeader *)&Sprite[i], // Object to Mark when animation is stepped;
						eGFX_ANIMATOR_MODE__FRACTIONAL_BISECT
						);

			if (i != 0)
				eGFX_AddAnimator(AnimatorList, &PointAnim[i]);


			eGFX_StartAnimators(AnimatorList);
		}
	}
	
	eGFX_ProcessObjects(ObjectList, eGFX_OBJECT_DRAW_CMD__RESTORE);

	eGFX_ProcessObjects(ObjectList, eGFX_OBJECT_DRAW_CMD__PAINT);
		
	eGFX_WaitForV_Sync();
	
	eGFX_Dump(&eGFX_BackBuffer[SpriteTestActiveBuffer]);


	return;
}

void Activity__Sprite_Exit(uint32_t MessageID_In, void *MessageIn)
{

	switch (MessageID_In)
	{
		//System Messages...   Do nothing if you don't need the behavior
	case	eGFX_ACTIVITY_MSG_ID_RESET:
		break;

	case	eGFX_ACTIVITY_MSG_ID_NO_MSG:
		break;

	case	eGFX_ACTIVITY_MSG_ID_POP:
		break;

	case	eGFX_ACTIVITY_MSG_ID_PUSH:
		break;

	case	eGFX_ACTIVITY_MSG_ID_SWITCH:
		break;

	default:
		break;
	}

}
