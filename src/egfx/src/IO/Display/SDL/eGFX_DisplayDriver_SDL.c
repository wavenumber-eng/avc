#include "eGFX.h"
#include "eGFX_DisplayDriver_SDL.h"
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#if eGFX_SDL_SIM_GRID == 1
#define TEXTURE_BUFFER_PIXEL_SIZE	(eGFX_SDL_SIM_GRID_PIXEL_SIZE + (eGFX_SDL_SIM_GRID_PIXEL_BORDER*2))
#else
#define TEXTURE_BUFFER_PIXEL_SIZE	1
#endif


uint32_t TextureBuffer[eGFX_PHYSICAL_SCREEN_SIZE_X * eGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE * TEXTURE_BUFFER_PIXEL_SIZE];

eGFX_ImagePlane TexturePlane = {
			.Data = (uint8_t *)TextureBuffer,
			.SizeX = eGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE,
			.SizeY = eGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE,
			.Type = eGFX_IMAGE_PLANE_32BPP_XRGB888
};


SDL_Window* eGFX_Window;
SDL_Renderer* eGFX_Renderer;
SDL_Texture* eGFX_Texture;

int KeyCode = 0;

uint32_t eGFX_SDL_Zoom = 1;

static int last_w,last_h;

char WindowTitle[64];

void UpdateSDL_WindowTitle()
{
	snprintf(WindowTitle, sizeof(WindowTitle), "%d x %d  Zoom:%d %dBPP",
						  eGFX_PHYSICAL_SCREEN_SIZE_X, 
						  eGFX_PHYSICAL_SCREEN_SIZE_Y,
						  eGFX_SDL_Zoom,
						  eGFX_IMAGE_PLANE_BITS_PER_PIXEL_FROM_TYPE(eGFX_DISPLAY_DRIVER_IMAGE_PLANE_TYPE)
		    );
}


int ProcessSDL_Events()
{

	int done = 0;

	SDL_Event event;
	   	 
	/* Check for events */
	while (SDL_PollEvent(&event))
	{
		//ToDo :  tie into Input system

		switch (event.type)
		{
			
			break;

				case SDL_KEYDOWN:
	
					KeyCode = event.key.keysym.scancode;

			break;


		case SDL_KEYUP:


			KeyCode = event.key.keysym.scancode;

			break;
			/* Mouse events */
		case	SDL_MOUSEMOTION: /**< Mouse moved */
		case		SDL_MOUSEBUTTONDOWN:       /**< Mouse button pressed */
		case		SDL_MOUSEBUTTONUP:          /**< Mouse button released */
		case		SDL_MOUSEWHEEL:             /**< Mouse wheel motion */
			break;


			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_CLOSE:   // exit game
					return -1;
					break;

				default:
						break;

					case SDL_WINDOWEVENT_RESIZED:
					{
		
							int w, h;

							SDL_GetWindowSize(eGFX_Window, &w, &h);
						


							if (w < last_w)
							{
								eGFX_SDL_Zoom = ((w) / (eGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE));
							}
							else if (w > last_w)
							{
								eGFX_SDL_Zoom = ((w + (eGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE) ) / (eGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE));
							}
							else
							{
							}


							if (eGFX_SDL_Zoom < 1)
							{
								eGFX_SDL_Zoom = 1;
							}
							
							w = eGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE * eGFX_SDL_Zoom;
							h = eGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE * eGFX_SDL_Zoom;



							SDL_SetWindowSize(eGFX_Window, w, h);

							SDL_GetWindowSize(eGFX_Window, &last_w, &last_h);


							UpdateSDL_WindowTitle();

							SDL_SetWindowTitle(eGFX_Window, WindowTitle);
		
					}
					break;
				}
				break;

		}
	}

	return done;
}

void eGFX_DeInitDriver()
{
	SDL_DestroyTexture(eGFX_Texture);
	SDL_DestroyRenderer(eGFX_Renderer);
	SDL_DestroyWindow(eGFX_Window);
	SDL_Quit();
}

eGFX_ImagePlane eGFX_BackBuffer[eGFX_NUM_BACKBUFFERS];

#if (eGFX_IMAGE_PLANE_BITS_PER_PIXEL_FROM_TYPE(eGFX_DISPLAY_DRIVER_IMAGE_PLANE_TYPE) == 1)

uint8_t eGFX_FrameBuffer[eGFX_NUM_BACKBUFFERS][eGFX_CALCULATE_1BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];

#elif (eGFX_IMAGE_PLANE_BITS_PER_PIXEL_FROM_TYPE(eGFX_DISPLAY_DRIVER_IMAGE_PLANE_TYPE) == 3)

uint8_t eGFX_FrameBuffer[eGFX_CALCULATE_3BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];

#elif (eGFX_IMAGE_PLANE_BITS_PER_PIXEL_FROM_TYPE(eGFX_DISPLAY_DRIVER_IMAGE_PLANE_TYPE) == 4)

uint8_t eGFX_FrameBuffer[eGFX_NUM_BACKBUFFERS][eGFX_CALCULATE_4BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];

#elif (eGFX_IMAGE_PLANE_BITS_PER_PIXEL_FROM_TYPE(eGFX_DISPLAY_DRIVER_IMAGE_PLANE_TYPE) == 8)

uint8_t eGFX_FrameBuffer[eGFX_NUM_BACKBUFFERS][eGFX_CALCULATE_8BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];

#elif (eGFX_IMAGE_PLANE_BITS_PER_PIXEL_FROM_TYPE(eGFX_DISPLAY_DRIVER_IMAGE_PLANE_TYPE) == 16)

uint8_t eGFX_FrameBuffer[eGFX_NUM_BACKBUFFERS][eGFX_CALCULATE_16BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];

#elif (eGFX_IMAGE_PLANE_BITS_PER_PIXEL_FROM_TYPE(eGFX_DISPLAY_DRIVER_IMAGE_PLANE_TYPE) == 24)

uint8_t eGFX_FrameBuffer[eGFX_NUM_BACKBUFFERS][eGFX_CALCULATE_24BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];

#elif (eGFX_IMAGE_PLANE_BITS_PER_PIXEL_FROM_TYPE(eGFX_DISPLAY_DRIVER_IMAGE_PLANE_TYPE) == 32)

uint8_t eGFX_FrameBuffer[eGFX_NUM_BACKBUFFERS][eGFX_CALCULATE_32BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];

#else 

#error "I need a value backbuffer image plane type"

#endif

static uint32_t InactiveBackBuffer = 0;

uint32_t eGFX_GetInactiveBackBuffer()
{
	return InactiveBackBuffer++;
}

eGFX_VSyncCallback_t *VSyncCallback;

void eGFX_InitDriver(eGFX_VSyncCallback_t VS)
{
	VSyncCallback = VS;

	

	for (int i = 0; i < eGFX_NUM_BACKBUFFERS; i++)
	{

		eGFX_ImagePlaneInit(&eGFX_BackBuffer[i],
			&eGFX_FrameBuffer[i][0],
			eGFX_PHYSICAL_SCREEN_SIZE_X,
			eGFX_PHYSICAL_SCREEN_SIZE_Y,
			eGFX_DISPLAY_DRIVER_IMAGE_PLANE_TYPE);

	}

	memset(eGFX_FrameBuffer, 0, sizeof(eGFX_FrameBuffer));

	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

	#ifndef eGFX_SDL_INITIAL_ZOOM
		eGFX_SDL_Zoom = 640 / eGFX_PHYSICAL_SCREEN_SIZE_X;
	#else
		eGFX_SDL_Zoom = eGFX_SDL_INITIAL_ZOOM;

		if (eGFX_SDL_Zoom == 0)
		{
			eGFX_SDL_Zoom = 640 / eGFX_PHYSICAL_SCREEN_SIZE_X;
		}
	#endif

	UpdateSDL_WindowTitle();

	eGFX_Window = SDL_CreateWindow(WindowTitle,
									SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
									eGFX_PHYSICAL_SCREEN_SIZE_X* eGFX_SDL_Zoom * TEXTURE_BUFFER_PIXEL_SIZE, 
									eGFX_PHYSICAL_SCREEN_SIZE_Y* eGFX_SDL_Zoom * TEXTURE_BUFFER_PIXEL_SIZE,
									SDL_WINDOW_RESIZABLE);    



	SDL_GetWindowSize(eGFX_Window, &last_w, &last_h);


    eGFX_Renderer = SDL_CreateRenderer(eGFX_Window, -1, SDL_RENDERER_SOFTWARE);
	
	eGFX_Texture = SDL_CreateTexture(eGFX_Renderer,
									SDL_PIXELFORMAT_ARGB8888,
									SDL_TEXTUREACCESS_STREAMING,
									eGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE, 
									eGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE);

	SDL_SetTextureBlendMode(eGFX_Texture, SDL_BLENDMODE_NONE);

	memset(TextureBuffer, 0x00, eGFX_PHYSICAL_SCREEN_SIZE_Y * eGFX_PHYSICAL_SCREEN_SIZE_X * sizeof(uint32_t));

	SDL_RenderClear(eGFX_Renderer);
	/*Update the renderer with the texture containing the rendered image*/
	SDL_RenderCopy(eGFX_Renderer, eGFX_Texture, NULL, NULL);

	SDL_RenderPresent(eGFX_Renderer);

	UpdateSDL_WindowTitle();
}

void eGFX_Dump(eGFX_ImagePlane *Image)
{
	eGFX_PixelState PS;
	int r, g, b;

	uint32_t TexturePixelColor = 0;
	
 	#if (eGFX_SDL_SIM_GRID == 1)
	
		eGFX_Box PixelBox;

		for (int k = 0; k < sizeof(TextureBuffer) / sizeof(uint32_t); k++)
		{
			TextureBuffer[k] = eGFX_SDL_SIM_GRID_BACKGROUND_COLOR;
		}

	#endif

	for (int y = 0; y < eGFX_PHYSICAL_SCREEN_SIZE_Y; y++)
	{
		for (int x = 0; x < eGFX_PHYSICAL_SCREEN_SIZE_X; x++)
		{
			PS = eGFX_GetPixel(Image, x, y);

			r = 0;
			g = 0;
			b = 0;

			switch (Image->Type)
			{
			case eGFX_IMAGE_PLANE_1BPP:

				if (PS)
				{
					TexturePixelColor = 0xFFFFFFFF;
				}
				else
				{
					TexturePixelColor = 0;
				}

				break;

			case eGFX_IMAGE_PLANE_4BPP:

				//ToDo  Test Visually

				PS &= 0xF;

				TexturePixelColor = eGFX_COLOR_RGB_TO_RGBX888(PS << 4, PS<<4 , PS << 4);

				break;


			case eGFX_IMAGE_PLANE_8BPP:

				//ToDo  Test Visually

				PS &= 0xFF;

				TexturePixelColor = eGFX_COLOR_RGB_TO_RGBX888(PS, PS ,PS);

				break;


			case eGFX_IMAGE_PLANE_8BPP_XRGB222:

				PS &= 0xFF;

				TexturePixelColor = ((PS & 0x30) >> 4) << (16 + 6) |
									((PS & 0x0C) >> 2) << (8 + 6) |
									((PS & 0x03)) << (6);


				TexturePixelColor += 0x1F1F1F;

				break;

			 



			case eGFX_IMAGE_PLANE_16BPP_RGB565:
				
				PS &= 0xFFFF;

				//ToDo  Test Visually and implement eGFX Macro

				r = (PS>>11) & (0x1F);
				g = (PS>>5) & (0x3F);
				b = (PS) & (0x1F);
				
				if (r)
					r = r << 3 | 0x7;
				if (g)
					g = g << 2 | 0x3;
				if (b)
					b = b << 3 | 0x7;

				TexturePixelColor = eGFX_COLOR_RGB_TO_RGBX888(r,g,b);

				break;

			case eGFX_IMAGE_PLANE_16BPP_BGR565:
				
				PS &= 0xFFFF;

				//ToDo  Test Visually and implement eGFX Macro

				b = (PS >> 11) & (0x1F);
				g = (PS >> 5) & (0x3F);
				r = (PS) & (0x1F);

				TexturePixelColor = eGFX_COLOR_RGB_TO_RGBX888(r, g, b);

				break;

			case eGFX_IMAGE_PLANE_16BPP_GBRG3553:

				PS &= 0xFFFF;

				//ToDo  Test Visually and implement eGFX Macro

				r = (PS >> 3) & (0x1F);
				b = (PS >> 8) & (0x1F);
				g = (PS&0x7) | (PS>>10) ;

				TexturePixelColor = eGFX_COLOR_RGB_TO_RGBX888(r, g, b);

				break;

			case eGFX_IMAGE_PLANE_16BPP_GRBG3553:

				PS &= 0xFFFF;

				//ToDo  Test Visually and implement eGFX Macro

				b = (PS >> 3) & (0x1F);
				r = (PS >> 8) & (0x1F);
				g = (PS & 0x7) | (PS >> 10);

				TexturePixelColor = eGFX_COLOR_RGB_TO_RGBX888(r, g, b);

				break;

			case eGFX_IMAGE_PLANE_32BPP:
			case eGFX_IMAGE_PLANE_32BPP_XRGB888:

				TexturePixelColor = PS | 0xFF000000;

				break;
			}


			#if (eGFX_SDL_SIM_GRID == 1)
				
				PixelBox.P1.X = eGFX_SDL_SIM_GRID_PIXEL_BORDER + (x * (TEXTURE_BUFFER_PIXEL_SIZE));
				PixelBox.P1.Y = eGFX_SDL_SIM_GRID_PIXEL_BORDER + (y * (TEXTURE_BUFFER_PIXEL_SIZE));

				PixelBox.P2.X = PixelBox.P1.X + eGFX_SDL_SIM_GRID_PIXEL_SIZE;
				PixelBox.P2.Y = PixelBox.P1.Y + eGFX_SDL_SIM_GRID_PIXEL_SIZE;

				eGFX_DrawFilledBox(&TexturePlane, &PixelBox, TexturePixelColor);
			#else			
				TextureBuffer[y * eGFX_PHYSICAL_SCREEN_SIZE_X + x] = TexturePixelColor;
			#endif

		}
	}
	
    SDL_UpdateTexture(eGFX_Texture, NULL, &TextureBuffer, eGFX_PHYSICAL_SCREEN_SIZE_X * sizeof(Uint32) * (TEXTURE_BUFFER_PIXEL_SIZE));

	SDL_RenderCopy(eGFX_Renderer, eGFX_Texture, NULL, NULL);

	SDL_RenderPresent(eGFX_Renderer);

	if (VSyncCallback != NULL)
	{
		VSyncCallback(Image);
	}

}

void eGFX_SetBacklight(uint8_t BacklightValue)
{



}


