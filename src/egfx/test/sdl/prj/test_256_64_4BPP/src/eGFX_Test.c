#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "eGFX.h"
#include "Sprites_4BPP.h"

#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/
#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

eGFX_AnimatorHeader* Animators;

eGFX_Point_Animator A[6];

eGFX_Scalar_Animator LogoAnimator;


eGFX_Point Line1;
eGFX_Point Line2;
eGFX_Point Line3;
eGFX_Point Line4;


eGFX_Point SeparatorStart;
eGFX_Point SeparatorEnd;

#define TITLE_Y  4
#define TITLE_X  96

#define INFO_Y			34
#define INFO_Y_SPACE	9

#define SEPARATOR_X		TITLE_X - 8


int32_t logo_scale_int;
int AnimState;

int main(int argc, char *argv[])
{

	eGFX_Init_ScalarAnimator(&LogoAnimator,
		0,
		255,
		&logo_scale_int, //Point this to the thing you want to animate
		120,      //Maximum allowed frames before forced convergence
		.15f, //The amount of percentage of the distance the point should move each step. SHould be small (<0.5)
		eGFX_ANIMATOR_MODE__FRACTIONAL_BISECT
	);

     eGFX_Init_PointAnimator(&A[0],
		(eGFX_Point){.X = 300, .Y = 64}, //Start
		(eGFX_Point){.X = TITLE_X, .Y = TITLE_Y}, //End
		&Line1, //Point this to the thing you want to animate
		1000,      //Maximum allowed frames before forced convergence
		(eGFX_PointF){.15f,.15f}, //The amount of percentage of the distance the point should move each step. SHould be small (<0.5)
	      eGFX_ANIMATOR_MODE__FRACTIONAL_BISECT
	);

	 eGFX_Init_PointAnimator(&A[1],
		 (eGFX_Point) { .X = 32, .Y = 88 }, //Start
		 (eGFX_Point) { .X = TITLE_X, .Y = INFO_Y + (0*INFO_Y_SPACE) }, //End
		 & Line2, //Point this to the thing you want to animate
		 1000,      //Maximum allowed frames before forced convergence
		 (eGFX_PointF) {.15f, .15f }, //The amount of percentage of the distance the point should move each step. SHould be small (<0.5)
		 eGFX_ANIMATOR_MODE__FRACTIONAL_BISECT
	 );

	 eGFX_Init_PointAnimator(&A[2],
		 (eGFX_Point) { .X = 32, .Y = 88 }, //Start
		 (eGFX_Point) { .X = 96, .Y = INFO_Y + (1*INFO_Y_SPACE)}, //End
		 &Line3, //Point this to the thing you want to animate
		 1000,      //Maximum allowed frames before forced convergence
		 (eGFX_PointF) {.15f, .15f }, //The amount of percentage of the distance the point should move each step. SHould be small (<0.5)
		 eGFX_ANIMATOR_MODE__FRACTIONAL_BISECT
	 );

	eGFX_Init_PointAnimator(&A[3],
		 (eGFX_Point) { .X = 32, .Y = 88 }, //Start
		 (eGFX_Point) { .X = TITLE_X, .Y = INFO_Y + (2*INFO_Y_SPACE) }, //End
		 &Line4, //Point this to the thing you want to animate
		 1000,      //Maximum allowed frames before forced convergence
		 (eGFX_PointF) {.15f, .15f }, //The amount of percentage of the distance the point should move each step. SHould be small (<0.5)
		 eGFX_ANIMATOR_MODE__FRACTIONAL_BISECT
	 );



     eGFX_Init_PointAnimator(&A[4],
		(eGFX_Point){.X = 300, .Y = 64}, //Start
		(eGFX_Point){.X = SEPARATOR_X, .Y = 4}, //End
		&SeparatorStart, //Point this to the thing you want to animate
		1000,      //Maximum allowed frames before forced convergence
		(eGFX_PointF){.15f,.15f}, //The amount of percentage of the distance the point should move each step. SHould be small (<0.5)
	      eGFX_ANIMATOR_MODE__FRACTIONAL_BISECT
	);

	 eGFX_Init_PointAnimator(&A[5],
		 (eGFX_Point) { .X = 200, .Y = 0 }, //Start
		 (eGFX_Point) { .X = SEPARATOR_X, .Y = 60  }, //End
		 & SeparatorEnd, //Point this to the thing you want to animate
		 1000,      //Maximum allowed frames before forced convergence
		 (eGFX_PointF) {.15f, .15f }, //The amount of percentage of the distance the point should move each step. SHould be small (<0.5)
		 eGFX_ANIMATOR_MODE__FRACTIONAL_BISECT
	 );

	eGFX_InitDriver(NULL);

	Animators = &A;

	eGFX_Animator_AddToList(Animators,&A[1]);
	eGFX_Animator_AddToList(Animators,&A[2]);
	eGFX_Animator_AddToList(Animators,&A[3]);
	eGFX_Animator_AddToList(Animators,&A[4]);
	eGFX_Animator_AddToList(Animators,&A[5]);
	eGFX_Animator_AddToList(Animators,&LogoAnimator);

	eGFX_StartAnimators(Animators);
	  
  	while (!ProcessSDL_Events()) 
  	{
		SDL_Delay(24);

		eGFX_ImagePlane_Clear(&eGFX_BackBuffer[0]);

		eGFX_Animator_ProcessList(Animators);

		eGFX_DrawStringColored(&eGFX_BackBuffer[0], "Synchro", Line1.X, Line1.Y, &FONT_10_14_1BPP,0xFF);
	
		eGFX_DrawStringColored(&eGFX_BackBuffer[0], "Serial: 10092", Line2.X, Line2.Y, &FONT_5_7_1BPP, 0xFF);
				
		eGFX_DrawStringColored(&eGFX_BackBuffer[0], "Firmware Version : v2.1", Line3.X, Line3.Y, &FONT_5_7_1BPP, 0xFF);
		eGFX_DrawStringColored(&eGFX_BackBuffer[0], "Bootloader : v1.1", Line4.X, Line4.Y, &FONT_5_7_1BPP, 0xFF);


		eGFX_DrawLine(&eGFX_BackBuffer[0], SeparatorStart.X, SeparatorStart.Y, SeparatorEnd.X, SeparatorEnd.Y, 0xFF);


		eGFX_BlitFaded(&eGFX_BackBuffer[0],8,4,&Sprite_4BPP_wn_w_text_83_56,logo_scale_int);

		AnimState++;
		if (AnimState > 80)
		{
				
			if (eGFX_AnimatorsAreComplete(Animators))
			{
			
				for (int i = 0; i < 6; i++)
				{
					A[i].End.Y = 350;
					A[i].Start = *(A[i].Current);
					A[i].CurrentFrame = 0;
				
					A[i].FractionToMove.X = .025;
					A[i].FractionToMove.Y = .025;

				}

				A[4].End.X = 400;
				A[4].End.Y = 64;
			
				A[4].FractionToMove.X = .05;
				A[4].FractionToMove.Y = .05;


				A[5].End.X = 400;
				A[5].End.Y = 0;
				
				
				A[5].FractionToMove.X = .15;
				A[5].FractionToMove.Y = .15;



				LogoAnimator.End = 0;
				LogoAnimator.CurrentFrame = 0;
				LogoAnimator.Start = *LogoAnimator.Current;
				eGFX_StartAnimators(Animators);
			}
		}

		eGFX_Dump(&eGFX_BackBuffer[0]);			
    }

	eGFX_DeInitDriver();
	
    return 0;
}
