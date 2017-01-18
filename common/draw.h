#pragma once

#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "text.h"


void InitDrawOverlay(unsigned int ScreenWidth, unsigned int ScreenHeight);
void LoadDrawIdentity();
void DrawCircle(float x, float y, float radiusx, float radiusy);
void DrawCircle(float x, float y, float radius);
void DrawLine(float sx, float sy, float ex, float ey);

void DrawRect(float x, float y, float width, float height);
void DrawOutlinedRect(float x, float y, float width, float height);

void DrawPoly(float x, float y, GLenum type, GLfloat buffer[], unsigned int num);

void SetDrawColor(short r, short g, short b, short a);
void SetDrawColor(short r, short g, short b);
void Scale(float x, float y);
void Scale(float scale);
void SetClip(float sx, float sy, float width, float height);
void ClearClip();
void PointSize(float size);
void LineWidth(float size);
void Translate(float x, float y);
void SetTranslation(float x, float y);
void Offset(float x, float y);
void Rotate(float degrees);
void SetRotation(float degrees);
