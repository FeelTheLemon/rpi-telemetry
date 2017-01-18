#pragma once

#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#include "gfx.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <cmath>
#include <algorithm>
#include <cassert>
#include <cwchar>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#define check() assert(glGetError() == 0)

void InitText(unsigned int ScreenWidth, unsigned int ScreenHeight);
void LoadTextIdentity();
void TranslateText(float x, float y);
void SetTextTranslation(float x, float y);
void RotateText(float angle);
void SetTextRotation(float angle);
void SetTextColor(short r, short g, short b, short a);
void SetTextColor(short r, short g, short b);
void DrawText(const char *text, float x, float y, float size);
void DrawTextShadowed(const char *text, float x, float y, float size);
void DrawWTextShadowed(const wchar_t *text, float x, float y, float size);