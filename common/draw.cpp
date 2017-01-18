#include "draw.h"

#define check() assert(glGetError() == 0)

extern unsigned int GScreenWidth;
extern unsigned int GScreenHeight;

GfxShader GDrawVS;
GfxShader GDrawFS;
GfxProgram GDrawProg;
extern GfxProgram GTextProg;

GLuint shColor;
GLuint shVertexSize;
GLuint shVertex;
GLuint shTranslation;
GLuint shOffset;
GLuint shRotation;
GLuint shScale;
GLuint shScreenSize;

GLuint dvbo;


void InitDrawOverlay(unsigned int ScreenWidth, unsigned int ScreenHeight) {
	GDrawVS.LoadVertexShader("shaders/draw.v.glsl");
	GDrawFS.LoadFragmentShader("shaders/draw.f.glsl");
	GDrawProg.Create(&GDrawVS, &GDrawFS);
	check();

	shColor = glGetUniformLocation(GDrawProg.GetId(), "color");

	shVertexSize = glGetUniformLocation(GDrawProg.GetId(), "size");
	shVertex = glGetAttribLocation(GDrawProg.GetId(), "vertex");
	shTranslation = glGetUniformLocation(GDrawProg.GetId(), "translation");
	shOffset = glGetUniformLocation(GDrawProg.GetId(), "offset");
	shRotation = glGetUniformLocation(GDrawProg.GetId(), "rotation");
	shScale = glGetUniformLocation(GDrawProg.GetId(), "scale");
	shScreenSize = glGetUniformLocation(GDrawProg.GetId(), "scrsize");
	check();

	printf("Overlay resolution: %ux%u\n", ScreenWidth, ScreenHeight);

	glUseProgram(GDrawProg.GetId());	check();
	
	glUniform2f(shScreenSize, ScreenWidth, ScreenHeight);
	check();

	glGenBuffers(1, &dvbo);
	check();

}

void LoadDrawIdentity() {
	Scale(1.0);
	PointSize(1.0);
	LineWidth(1.0);
	SetTranslation(0.0, 0.0);
	Offset(0.0, 0.0);
	SetRotation(0.0);
	SetDrawColor(255, 255, 255, 255);
	ClearClip();
	check();
}

void SetDrawColor(short r, short g, short b, short a) {
	if (a < 255) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_DST_COLOR);
	}
	glUseProgram(GDrawProg.GetId());
	glUniform4f(shColor, (float)r / 255, (float)g / 255, (float)b / 255, (float)a / 255);
	check();
}
void SetDrawColor(short r, short g, short b) {
	SetDrawColor(r, g, b, 255);
}

float scalex = 0;
float scaley = 0;
void Scale(float x, float y) {
	scalex = x;
	scaley = y;
	glUseProgram(GDrawProg.GetId());
	glUniform2f(shScale, scalex, scaley);
	check();
}
void Scale(float scale) {
	Scale(scale, scale);
}

void PointSize(float size) {
	glUseProgram(GDrawProg.GetId());
	glUniform1f(shVertexSize, size);
	check();
}

void LineWidth(float size) {
	glLineWidth(size);
	check();
}

float transx = 0;
float transy = 0;
void Translate(float x, float y) {
	transx += x;
	transy += y;
	glUseProgram(GDrawProg.GetId());
	glUniform2f(shTranslation, transx, transy);
	check();
}
void SetTranslation(float x, float y) {
	transx = x;
	transy = y;
	glUseProgram(GDrawProg.GetId());
	glUniform2f(shTranslation, transx, transy);
	check();
}

void SetClip(float sx, float sy, float width, float height) {
	glEnable(GL_SCISSOR_TEST);
	glScissor(sx + transx, sy + transy, width, height);
	check();
}
void ClearClip() {
	glDisable(GL_SCISSOR_TEST);
	check();
}

float offsetx = 0;
float offsety = 0;
void Offset(float x, float y) {
	offsetx = x;
	offsety = y;
	glUseProgram(GDrawProg.GetId());
	glUniform2f(shOffset, offsetx, offsety);
	check();
}

float rotdeg = 0;
void Rotate(float degrees) {
	rotdeg += degrees;
	glUseProgram(GDrawProg.GetId());
	glUniform1f(shRotation, rotdeg);
	check();
}
void SetRotation(float degrees) {
	rotdeg = degrees;
	glUseProgram(GDrawProg.GetId());
	glUniform1f(shRotation, rotdeg);
	check();
}

GLfloat circle[] = {
	0.0f, 1.0f,
	0.1736f, 0.9848f,
	0.342f, 0.9396f,
	0.4999f, 0.866f,
	0.6427f, 0.766f,
	0.766f, 0.6427f,
	0.866f, 0.5f,
	0.9396f, 0.342f,
	0.9848f, 0.1736f,
	1.0f, 0.0f,
	0.9848f, -0.1737f,
	0.9396f, -0.3421f,
	0.866f, -0.5f,
	0.766f, -0.6428f,
	0.6427f, -0.7661f,
	0.4999f, -0.8661f,
	0.342f, -0.9397f,
	0.1736f, -0.9849f,
	0.0f, -1.0f,
	-0.1737f, -0.9849f,
	-0.3421f, -0.9397f,
	-0.5001f, -0.8661f,
	-0.6428f, -0.7661f,
	-0.7661f, -0.6428f,
	-0.8661f, -0.5001f,
	-0.9397f, -0.3421f,
	-0.9849f, -0.1737f,
	-1.0f, -0.0001f,
	-0.9849f, 0.1736f,
	-0.9397f, 0.342f,
	-0.8661f, 0.5f,
	-0.7661f, 0.6427f,
	-0.6428f, 0.766f,
	-0.5001f, 0.866f,
	-0.3421f, 0.9396f,
	-0.1737f, 0.9848f,
	-0.0001f, 1.0f,
};

void DrawCircle(float x, float y, float radiusx, float radiusy) {
	glUseProgram(GDrawProg.GetId());

	glEnableVertexAttribArray(shVertex);
	glBindBuffer(GL_ARRAY_BUFFER, dvbo);
	glVertexAttribPointer(
		shVertex, //vertexPosition_modelspaceID, // The attribute we want to configure
		2,                  // size
		GL_FLOAT,           // type
		false,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);	check();

	glUniform2f(shScale, radiusx, radiusy);
	glUniform2f(shOffset, x / radiusx, y / radiusy);

	glBufferData(GL_ARRAY_BUFFER, sizeof circle, circle, GL_DYNAMIC_DRAW);
	glDrawArrays(GL_LINE_LOOP, 0, 37);

	glDisableVertexAttribArray(shVertex);
	glUniform2f(shOffset, offsetx, offsety);
	glUniform2f(shScale, scalex, scaley);
	check();
}
void DrawCircle(float x, float y, float radius) {
	DrawCircle(x, y, radius, radius);
}

void DrawLine(float sx, float sy, float ex, float ey){
	GLfloat line[] = {
	 sx, sy,
	 ex, ey
	};

	glUseProgram(GDrawProg.GetId());
	glBindBuffer(GL_ARRAY_BUFFER, dvbo);
	glEnableVertexAttribArray(shVertex);
	glVertexAttribPointer(
		shVertex, //vertexPosition_modelspaceID, // The attribute we want to configure
		2,                  // size
		GL_FLOAT,           // type
		false,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glBufferData(GL_ARRAY_BUFFER, sizeof line, line, GL_DYNAMIC_DRAW);
	glDrawArrays(GL_LINES, 0, 2);

	glDisableVertexAttribArray(shVertex);

	check();
}



void DrawRect(float x, float y, float width, float height) {
	GLfloat rect[] = {
	 x, y,
	 x, y + height,
 	 x + width, y,
	 x + width, y + height,
	};

	glUseProgram(GDrawProg.GetId());
	glBindBuffer(GL_ARRAY_BUFFER, dvbo);
	glEnableVertexAttribArray(shVertex);
	glVertexAttribPointer(
		shVertex, //vertexPosition_modelspaceID, // The attribute we want to configure
		2,                  // size
		GL_FLOAT,           // type
		false,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glBufferData(GL_ARRAY_BUFFER, sizeof rect, rect, GL_DYNAMIC_DRAW);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableVertexAttribArray(shVertex);
	check();
}

void DrawOutlinedRect(float x, float y, float width, float height) {
	GLfloat rect[] = {
	 x, y,
	 x, y + height,
	 x + width, y + height,
	 x + width, y,
	};

	glUseProgram(GDrawProg.GetId());
	glBindBuffer(GL_ARRAY_BUFFER, dvbo);
	glEnableVertexAttribArray(shVertex);
	glVertexAttribPointer(
		shVertex, //vertexPosition_modelspaceID, // The attribute we want to configure
		2,                  // size
		GL_FLOAT,           // type
		false,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glBufferData(GL_ARRAY_BUFFER, sizeof rect, rect, GL_DYNAMIC_DRAW);
	glDrawArrays(GL_LINE_LOOP, 0, 4);

	glDisableVertexAttribArray(shVertex);
	check();
}

void DrawPoly(float x, float y, GLenum type, GLfloat buffer[], unsigned int num) {
	glUseProgram(GDrawProg.GetId());
	glBindBuffer(GL_ARRAY_BUFFER, dvbo);
	glEnableVertexAttribArray(shVertex);
	glVertexAttribPointer(
		shVertex, //vertexPosition_modelspaceID, // The attribute we want to configure
		2,                  // size
		GL_FLOAT,           // type
		false,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glUniform2f(shOffset, x, y);
	
	glBufferData(GL_ARRAY_BUFFER, num * 2 * sizeof(GLfloat), buffer, GL_DYNAMIC_DRAW);
	glDrawArrays(type, 0, num);

	glUniform2f(shOffset, offsetx, offsety);
	glDisableVertexAttribArray(shVertex);
	check();
}
