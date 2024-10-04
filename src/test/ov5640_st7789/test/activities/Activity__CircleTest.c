#include "eGFX.h"
#include "ASSETS/Sprites/Sprites_16BPP_RGB565.h""
#include "math.h"

static int Cnt = 0;

#define NUM_CIRCLES	18

eGFX_Obj_Circle Circle[NUM_CIRCLES];
eGFX_Point_Animator PointAnim[NUM_CIRCLES];
eGFX_Scalar_Animator ScalarAnim[NUM_CIRCLES];

#define FRAC_X	0.16f
#define FRAC_Y  0.16f
#define MAX_FRAMES	200
#define ANIM_MODE_POS eGFX_ANIMATOR_MODE__FRACTIONAL_BISECT
#define ANIM_MODE_RAD eGFX_ANIMATOR_MODE__LINEAR

uint32_t AnimPhase = 0;

static void *ObjectList = &Circle[0];

static void *AnimatorList = &PointAnim[0];

static uint32_t CircleTestActiveBuffer = 0;

static eGFX_ImagePlane * BackgroundImage;

static eGFX_ImagePlane * ForegroundImage;

static eGFX_PixelState BackgroundColor = 0;

void Activity__Circle_Enter(uint32_t MessageID, void *Message)
{

	BackgroundImage = (eGFX_ImagePlane *)(Message);
	BackgroundColor = MessageID;


	CircleTestActiveBuffer = 0;

	ForegroundImage = &eGFX_BackBuffer[CircleTestActiveBuffer];


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

	for (int i = 0; i < NUM_CIRCLES; i++)
	{
		eGFX_InitObject_Circle(&Circle[i], 0, &ForegroundImage);

		if (eGFX_NUM_BACKBUFFERS == 2)
			Circle[i].DoubleBuffered = true;
		else
			Circle[i].DoubleBuffered = false;

		if (i != 0)
			eGFX_AddObject(ObjectList, &Circle[i]);

		Circle[i].Filled = true;

		if (i & 0x01)
			Circle[i].Color = 0x25DB;
		else
			Circle[i].Color = 0xF4E1;

		Circle[i].Radius = 0;

		if (Message != NULL)
		{
			Circle[i].Header.DirtyRectangeRestore = eGFX_DIRTY_RECTANGLE_RESTORE_BACKGROUND_IMAGE;


			Circle[i].Header.BackgroundImage = &BackgroundImage;
		}
		else
		{

			Circle[i].Header.DirtyRectangeRestore = eGFX_DIRTY_RECTANGLE_RESTORE_BACKGROUND_COLOR;
			Circle[i].Header.BackgroundColor = &BackgroundColor;
		}

		eGFX_Init_PointAnimator(&PointAnim[i],
			Circle[i].Position,
			(eGFX_Point) {
			rand() % 240 + 120, rand() % 240 + 120
		},
			&(Circle[i].Position), //Point this to the thing you want to animate
				MAX_FRAMES,      //Maximum allowed frames before forced convergence
				(eGFX_PointF) {
				FRAC_X, FRAC_Y
			}, //The amount of percentage of the distance the point should move each step. Values <=0 or >= 1 are set to 0.5
				(eGFX_ObjectHeader *)&Circle[i], // Object to Mark when animation is stepped;
					ANIM_MODE_POS
					);


		eGFX_Init_ScalarAnimator(&ScalarAnim[i],
			Circle[i].Radius,
			10,
			&Circle[i].Radius, //Point this to the thing you want to animate
			25,     //Maximum allowed frames before forced convergence
			0.1f,//The amount of percentage of the distance the point should move each step. Values <=0 or >= 1 are set to 0.5
			(eGFX_ObjectHeader *)&Circle[i], // Object to Mark when animation is stepped;
			ANIM_MODE_RAD
		);

		if (i != 0)
			eGFX_AddAnimator(AnimatorList, &PointAnim[i]);
	}

	for (int i = 0; i < NUM_CIRCLES; i++)
	{
		eGFX_AddAnimator(AnimatorList, &ScalarAnim[i]);
	}

	eGFX_StartAnimators(AnimatorList);



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


void Activity__Circle_Process()
{

	if (eGFX_NUM_BACKBUFFERS == 2)
	{
		CircleTestActiveBuffer++;

		CircleTestActiveBuffer &= 0x01;
	}
	else
	{
		CircleTestActiveBuffer = 0;
	}
	ForegroundImage = &eGFX_BackBuffer[CircleTestActiveBuffer];

	eGFX_ProcessObjects(ObjectList, eGFX_OBJECT_DRAW_CMD__RESTORE);

	eGFX_ProcessObjects(ObjectList, eGFX_OBJECT_DRAW_CMD__PAINT);

	eGFX_ProcessAnimators(AnimatorList);


	if (eGFX_AnimatorsAreComplete(AnimatorList) == eGFX_TRUE)
	{

		AnimPhase++;
		if (AnimPhase >= 7)
		{
			AnimPhase = 0;

		}

		if (AnimPhase == 1)
		{
			for (int i = 0; i < NUM_CIRCLES; i++)
			{

				float x = 140.0f * cosf((float)i * M_PI * 2.0f / (float)NUM_CIRCLES) + eGFX_PHYSICAL_SCREEN_SIZE_X / 2;
				float y = 140.0f * sinf((float)i * M_PI * 2.0f / (float)NUM_CIRCLES) + eGFX_PHYSICAL_SCREEN_SIZE_Y / 2;

				eGFX_Init_PointAnimator(&PointAnim[i],
					Circle[i].Position,
					(eGFX_Point) {
					x, y
				},
					&(Circle[i].Position), //Point this to the thing you want to animate
						MAX_FRAMES,      //Maximum allowed frames before forced convergence
						(eGFX_PointF) {
						FRAC_X, FRAC_Y
					}, //The amount of percentage of the distance the point should move each step. Values <=0 or >= 1 are set to 0.5
						(eGFX_ObjectHeader *)&Circle[i], // Object to Mark when animation is stepped;
							ANIM_MODE_POS
							);

				eGFX_Init_ScalarAnimator(&ScalarAnim[i],
					Circle[i].Radius,
					8,
					&Circle[i].Radius, //Point this to the thing you want to animate
					20,     //Maximum allowed frames before forced convergence
					0.1f,//The amount of percentage of the distance the point should move each step. Values <=0 or >= 1 are set to 0.5
					(eGFX_ObjectHeader *)&Circle[i], // Object to Mark when animation is stepped;
					ANIM_MODE_RAD
				);
			}
		}
		else if (AnimPhase == 2)
		{
			for (int i = 0; i < NUM_CIRCLES; i++)
			{

				float x = 180.0f * cosf((float)(i - 1) * M_PI * 2.0f / (float)NUM_CIRCLES) + eGFX_PHYSICAL_SCREEN_SIZE_X / 2;
				float y = 180.0f * sinf((float)(i - 1) * M_PI * 2.0f / (float)NUM_CIRCLES) + eGFX_PHYSICAL_SCREEN_SIZE_Y / 2;

				eGFX_Init_PointAnimator(&PointAnim[i],
					Circle[i].Position,
					(eGFX_Point) {
					x, y
				},
					&(Circle[i].Position), //Point this to the thing you want to animate
						MAX_FRAMES,      //Maximum allowed frames before forced convergence
						(eGFX_PointF) {
						FRAC_X, FRAC_Y
					}, //The amount of percentage of the distance the point should move each step. Values <=0 or >= 1 are set to 0.5
						(eGFX_ObjectHeader *)&Circle[i], // Object to Mark when animation is stepped;
							ANIM_MODE_POS
							);

				eGFX_Init_ScalarAnimator(&ScalarAnim[i],
					Circle[i].Radius,
					9,
					&Circle[i].Radius, //Point this to the thing you want to animate
					30,     //Maximum allowed frames before forced convergence
					0.1f,//The amount of percentage of the distance the point should move each step. Values <=0 or >= 1 are set to 0.5
					(eGFX_ObjectHeader *)&Circle[i], // Object to Mark when animation is stepped;
					ANIM_MODE_RAD
				);
			}
		}
		else if (AnimPhase == 3)
		{
			for (int i = 0; i < NUM_CIRCLES; i++)
			{

				float x = 180.0f * cosf((float)(i + 2) * M_PI * 2.0f / (float)NUM_CIRCLES) + eGFX_PHYSICAL_SCREEN_SIZE_X / 2;
				float y = 180.0f * sinf((float)(i + 2) * M_PI * 2.0f / (float)NUM_CIRCLES) + eGFX_PHYSICAL_SCREEN_SIZE_Y / 2;

				eGFX_Init_PointAnimator(&PointAnim[i],
					Circle[i].Position,
					(eGFX_Point) {
					x, y
				},
					&(Circle[i].Position), //Point this to the thing you want to animate
						MAX_FRAMES,      //Maximum allowed frames before forced convergence
						(eGFX_PointF) {
						FRAC_X, FRAC_Y
					}, //The amount of percentage of the distance the point should move each step. Values <=0 or >= 1 are set to 0.5
						(eGFX_ObjectHeader *)&Circle[i], // Object to Mark when animation is stepped;
							ANIM_MODE_POS
							);

				eGFX_Init_ScalarAnimator(&ScalarAnim[i],
					Circle[i].Radius,
					9,
					&Circle[i].Radius, //Point this to the thing you want to animate
					30,     //Maximum allowed frames before forced convergence
					0.1,//The amount of percentage of the distance the point should move each step. Values <=0 or >= 1 are set to 0.5
					(eGFX_ObjectHeader *)&Circle[i], // Object to Mark when animation is stepped;
					ANIM_MODE_RAD
				);
			}
		}
		else if (AnimPhase == 4)
		{
			for (int i = 0; i < NUM_CIRCLES; i++)
			{

				float x = 100.0 * cosf((float)(NUM_CIRCLES / 2 - i - 1) * M_PI * 2.0f / (float)NUM_CIRCLES) + eGFX_PHYSICAL_SCREEN_SIZE_X / 2;
				float y = 100.0 * sinf((float)(NUM_CIRCLES / 2 - i - 1) * M_PI * 2.0f / (float)NUM_CIRCLES) + eGFX_PHYSICAL_SCREEN_SIZE_Y / 2;


				eGFX_Init_PointAnimator(&PointAnim[i],
					Circle[i].Position,
					(eGFX_Point) {
					x, y
				},
					&(Circle[i].Position), //Point this to the thing you want to animate
						MAX_FRAMES,      //Maximum allowed frames before forced convergence
						(eGFX_PointF) {
						FRAC_X, FRAC_Y
					}, //The amount of percentage of the distance the point should move each step. Values <=0 or >= 1 are set to 0.5
						(eGFX_ObjectHeader *)&Circle[i], // Object to Mark when animation is stepped;
							ANIM_MODE_POS
							);

				eGFX_Init_ScalarAnimator(&ScalarAnim[i],
					Circle[i].Radius,
					5,
					&Circle[i].Radius, //Point this to the thing you want to animate
					30,     //Maximum allowed frames before forced convergence
					0.1f,//The amount of percentage of the distance the point should move each step. Values <=0 or >= 1 are set to 0.5
					(eGFX_ObjectHeader *)&Circle[i], // Object to Mark when animation is stepped;
					ANIM_MODE_RAD
				);
			}
		}
		else if (AnimPhase == 5)
		{
			for (int i = 0; i < NUM_CIRCLES; i++)
			{

				float x = i * (eGFX_PHYSICAL_SCREEN_SIZE_X / (NUM_CIRCLES)) + (eGFX_PHYSICAL_SCREEN_SIZE_X / (NUM_CIRCLES)) / 2;
				float y = eGFX_PHYSICAL_SCREEN_SIZE_Y / 2;


				eGFX_Init_PointAnimator(&PointAnim[i],
					Circle[i].Position,
					(eGFX_Point) {
					x, y
				},
					&(Circle[i].Position), //Point this to the thing you want to animate
						MAX_FRAMES,      //Maximum allowed frames before forced convergence
						(eGFX_PointF) {
						FRAC_X, FRAC_Y
					}, //The amount of percentage of the distance the point should move each step. Values <=0 or >= 1 are set to 0.5
						(eGFX_ObjectHeader *)&Circle[i], // Object to Mark when animation is stepped;
							ANIM_MODE_POS
							);

				eGFX_Init_ScalarAnimator(&ScalarAnim[i],
					Circle[i].Radius,
					5,
					&Circle[i].Radius, //Point this to the thing you want to animate
					30,     //Maximum allowed frames before forced convergence
					0.1,//The amount of percentage of the distance the point should move each step. Values <=0 or >= 1 are set to 0.5
					(eGFX_ObjectHeader *)&Circle[i], // Object to Mark when animation is stepped;
					ANIM_MODE_RAD
				);
			}
		}
		else if (AnimPhase == 0)
		{
			for (int i = 0; i < NUM_CIRCLES; i++)
			{

				float x = 0 * cosf((float)i * M_PI * 2.0f / (float)NUM_CIRCLES) + eGFX_PHYSICAL_SCREEN_SIZE_X / 2;
				float y = 0 * sinf((float)i * M_PI * 2.0f / (float)NUM_CIRCLES) + eGFX_PHYSICAL_SCREEN_SIZE_Y / 2;

				eGFX_Init_PointAnimator(&PointAnim[i],
					Circle[i].Position,
					(eGFX_Point) {
					x, y
				},
					&(Circle[i].Position), //Point this to the thing you want to animate
						MAX_FRAMES,      //Maximum allowed frames before forced convergence
						(eGFX_PointF) {
						FRAC_X, FRAC_Y
					}, //The amount of percentage of the distance the point should move each step. Values <=0 or >= 1 are set to 0.5
						(eGFX_ObjectHeader *)&Circle[i], // Object to Mark when animation is stepped;
							ANIM_MODE_POS
							);

				eGFX_Init_ScalarAnimator(&ScalarAnim[i],
					Circle[i].Radius,
					1,
					&Circle[i].Radius, //Point this to the thing you want to animate
					20,     //Maximum allowed frames before forced convergence
					0.1,//The amount of percentage of the distance the point should move each step. Values <=0 or >= 1 are set to 0.5
					(eGFX_ObjectHeader *)&Circle[i], // Object to Mark when animation is stepped;
					ANIM_MODE_RAD
				);
			}
		}
		else
		{
			for (int i = 0; i < NUM_CIRCLES; i++)
			{
				eGFX_Init_PointAnimator(&PointAnim[i],
					Circle[i].Position,
					(eGFX_Point) {
					(rand() % 480), (rand() % 480)
				},
					&(Circle[i].Position), //Point this to the thing you want to animate
						MAX_FRAMES,      //Maximum allowed frames before forced convergence
						(eGFX_PointF) {
						FRAC_X, FRAC_Y
					}, //The amount of percentage of the distance the point should move each step. Values <=0 or >= 1 are set to 0.5
						(eGFX_ObjectHeader *)&Circle[i], // Object to Mark when animation is stepped;
							ANIM_MODE_POS
							);

				eGFX_Init_ScalarAnimator(&ScalarAnim[i],
					Circle[i].Radius,
					rand() % 15,
					&Circle[i].Radius, //Point this to the thing you want to animate
					20,     //Maximum allowed frames before forced convergence
					0.1,//The amount of percentage of the distance the point should move each step. Values <=0 or >= 1 are set to 0.5
					(eGFX_ObjectHeader *)&Circle[i], // Object to Mark when animation is stepped;
					ANIM_MODE_RAD
				);
			}
		}

		eGFX_StartAnimators(AnimatorList);
	}

	eGFX_WaitForV_Sync();

	eGFX_Dump(&eGFX_BackBuffer[CircleTestActiveBuffer]);


	return;
}

void Activity__Circle_Exit(uint32_t MessageID_In, void *MessageIn)
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
