#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <clocale>

#include "common/camera.h"

#include "common/gfx.h"
#include "common/graphics.h"

#include <time.h>

#include "overlay.h"
#include "data.h"

#define MAIN_TEXTURE_WIDTH 928 //16*60 768 // 16*48    // 704*1024 stretches, provides 6 levels, offsets red one along
#define MAIN_TEXTURE_HEIGHT 544  //16*40 512  // 16*32

//#define MAIN_TEXTURE_WIDTH 32 * 29
//#define MAIN_TEXTURE_HEIGHT 32 * 17

// MAIN_TEXTURE is YUV pixels not screen pixels...

float FPS = 0;
clock_t current_ticks, delta_ticks;

//entry point
int main(int argc, const char **argv)
{
	//setlocale(LC_ALL, "");

	InitData();

	//init graphics and the camera
	InitGraphics();
	
	CCamera* cam = StartCamera(MAIN_TEXTURE_WIDTH, MAIN_TEXTURE_HEIGHT,30,1,false);

	GfxTexture ytexture,utexture,vtexture,rgbtexture;

	ytexture.CreateGreyScale(MAIN_TEXTURE_WIDTH,MAIN_TEXTURE_HEIGHT);
	utexture.CreateGreyScale(MAIN_TEXTURE_WIDTH/2,MAIN_TEXTURE_HEIGHT/2);
	vtexture.CreateGreyScale(MAIN_TEXTURE_WIDTH/2,MAIN_TEXTURE_HEIGHT/2);

	rgbtexture.CreateRGBA(MAIN_TEXTURE_WIDTH,MAIN_TEXTURE_HEIGHT); // 192,128);
	rgbtexture.GenerateFrameBuffer();

	InitOverlay();

	printf("Running frame loop\n");

	while(1)
	{
		
		//spin until we have a camera frame
		const void* frame_data; int frame_sz;
		while(!cam->BeginReadFrame(0,frame_data,frame_sz)) 
		{

		}
		//lock the chosen frame buffer, and copy it directly into the corresponding open gl texture
		{
			const uint8_t* data = (const uint8_t*)frame_data;
			int ypitch = MAIN_TEXTURE_WIDTH;
			int ysize = ypitch*MAIN_TEXTURE_HEIGHT;
			int uvpitch = MAIN_TEXTURE_WIDTH/2;
			int uvsize = uvpitch*MAIN_TEXTURE_HEIGHT/2;
			//int upos = ysize+16*uvpitch;
			//int vpos = upos+uvsize+4*uvpitch;
			int upos = ysize;
			int vpos = upos+uvsize;
			//	printf("Frame data len: 0x%x, ypitch: 0x%x ysize: 0x%x, uvpitch: 0x%x, uvsize: 0x%x, u at 0x%x, v at 0x%x, total 0x%x\n", frame_sz, ypitch, ysize, uvpitch, uvsize, upos, vpos, vpos+uvsize);
			ytexture.SetPixels(data);
			utexture.SetPixels(data+upos);
			vtexture.SetPixels(data+vpos);
			cam->EndReadFrame(0);
		}


		//begin frame, draw the texture then end frame (the bit of maths just fits the image to the screen while maintaining aspect ratio)
		BeginFrame();

			
			DrawYUVTextureRect(&ytexture,&utexture,&vtexture,1.f,1.f,-1.f,-1.f,NULL,0);
			//DrawTextureRect(&rgbtexture,-1,-1,1,1,NULL,0,0.0,0.0);
			
			DrawOverlay();

		EndFrame();

		FPS = (float)CLOCKS_PER_SEC / (float)(clock() - current_ticks);
		current_ticks = clock();
	}

	StopCamera();
  return 0;
}

