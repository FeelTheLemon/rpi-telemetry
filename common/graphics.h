#pragma once

#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#include "gfx.h"

void InitGraphics();
void ReleaseGraphics();
void BeginFrame();
void EndFrame();


void SaveFrameBuffer(const char* fname);

void DrawTextureRect(GfxTexture* texture, float x0, float y0, float x1, float y1, GfxTexture* render_target, int whichTexture, float x, float y);
void DrawYUVTextureRect(GfxTexture* ytexture, GfxTexture* utexture, GfxTexture* vtexture, float x0, float y0, float x1, float y1, GfxTexture* render_target, int whichTexture);

void DrawOverlay();