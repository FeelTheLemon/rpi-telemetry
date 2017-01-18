


#include "text.h"

// Maximum texture width
#define MAXWIDTH 2048
//unsigned int MAXWIDTH;
#define TEXTSIZE 32

struct point {
	GLfloat x;
	GLfloat y;
	GLfloat s;
	GLfloat t;
};

extern unsigned int GScreenWidth;
extern unsigned int GScreenHeight;

GLuint vbo;

FT_Library ft;
FT_Face face;

GLint attribute_coord;
GLint uniform_tex;
GLint uniform_color;
GLint uniform_translation;
GLint uniform_offset;
GLint uniform_rotation;
GLint uniform_scrsize;

GfxShader GTextVS;
GfxShader GTextFS;
GfxProgram GTextProg;


/**
 * The atlas struct holds a texture that contains the visible US-ASCII characters
 * of a certain font rendered with a certain character height.
 * It also contains an array that contains all the information necessary to
 * generate the appropriate vertex and texture coordinates for each character.
 *
 * After the constructor is run, you don't need to use any FreeType functions anymore.
 */
struct atlas {
	GLuint tex;		// texture object
	GfxTexture texture;
	unsigned int w;			// width of texture in pixels
	unsigned int h;			// height of texture in pixels

	struct {
		float ax;	// advance.x
		float ay;	// advance.y

		float bw;	// bitmap.width;
		float bh;	// bitmap.height;

		float bl;	// bitmap_left;
		float bt;	// bitmap_top;

		float tx;	// x offset of glyph in texture coordinates
		float ty;	// y offset of glyph in texture coordinates
	} c[65535];		// character information

	 atlas(FT_Face face, int height) {
		FT_Set_Pixel_Sizes(face, 0, height);
		FT_GlyphSlot g = face->glyph;

		unsigned int roww = 0;
		unsigned int rowh = 0;
		 w = 0;
		 h = 0;

		 memset(c, 0, sizeof c);

		/* Find minimum size for a texture holding all visible ASCII characters */
		for (int i = 0; i < 2048; i++) {
			if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
				fprintf(stderr, "Loading character %c failed!\n", i);
				continue;
			}
			if (roww + g->bitmap.width + 1 >= MAXWIDTH) {
				w = std::max(w, roww);
				h += rowh;
				roww = 0;
				rowh = 0;
			}
			roww += g->bitmap.width + 1;
			rowh = std::max(rowh, g->bitmap.rows);
		}

		w = std::max(w, roww);
		h += rowh;

		
/*		
		glActiveTexture(GL_TEXTURE2);
		check();
		glGenTextures(1, &tex);
		check();
		glBindTexture(GL_TEXTURE_2D, tex);
		check();
		glUniform1i(uniform_tex, 0);
		check();

		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);
*/

		//printf("Created texture w: %u h: %u\n", w, h);
		texture.CreateGreyScaleAlpha(w,h);
		//texture.CreateGreyScaleAlpha(MAXWIDTH,MAXWIDTH);
		check();
		


		glBindTexture(GL_TEXTURE_2D, texture.GetId());
		//glUniform1i(uniform_tex, 0);

		
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		check();

		int ox = 0;
		int oy = 0;

		rowh = 0;

		//0x0020-0x007F латиница
		//0x0080-0x00FF доп латиница
		//0x02B0-0x02FF символы
		//0x0300-0x036F символы 2
		//0x2000-0x206F
		//0x0400-0x04FF киррилица

		for (int i = 0; i < 2048; i++) {
			if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
				fprintf(stderr, "Loading character %c failed!\n", i);
				continue;
			}

			if (ox + g->bitmap.width + 1 >= MAXWIDTH) {
				oy += rowh;
				rowh = 0;
				ox = 0;
			}

			glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);
			c[i].ax = g->advance.x >> 6;
			c[i].ay = g->advance.y >> 6;

			c[i].bw = g->bitmap.width;
			c[i].bh = g->bitmap.rows;

			c[i].bl = g->bitmap_left;
			c[i].bt = g->bitmap_top;

			c[i].tx = ox / (float)w;
			c[i].ty = oy / (float)h;

			rowh = std::max(rowh, g->bitmap.rows);
			ox += g->bitmap.width + 1;
		}

		check();
		fprintf(stderr, "Generated a %d x %d (%d kb) texture atlas\n", w, h, w * h / 1024);
	}

	~atlas() {
		glDeleteTextures(1, &tex);
	}


};

atlas *atl;

void InitText(unsigned int ScreenWidth, unsigned int ScreenHeight) {

	//check();

	/* Initialize the FreeType2 library */
	if (FT_Init_FreeType(&ft)) {
		fprintf(stderr, "Could not init freetype library\n");
	}

	/* Load a font */
	if (FT_New_Face(ft, "fonts/Roboto-Regular.ttf", 0, &face)) {
		fprintf(stderr, "Could not open font %s\n", "fonts/Roboto-Regular.ttf");
	}


	GTextVS.LoadVertexShader("shaders/text.v.glsl");
	GTextFS.LoadFragmentShader("shaders/text.f.glsl");
	GTextProg.Create(&GTextVS,&GTextFS);

	check();

	attribute_coord = glGetAttribLocation(GTextProg.GetId(),"coord");
	uniform_tex = glGetUniformLocation(GTextProg.GetId(),"tex");
	uniform_color = glGetUniformLocation(GTextProg.GetId(),"color");
	uniform_rotation = glGetUniformLocation(GTextProg.GetId(),"rotation");
	uniform_scrsize = glGetUniformLocation(GTextProg.GetId(),"scrsize");
	uniform_offset = glGetUniformLocation(GTextProg.GetId(),"offset");
	uniform_translation = glGetUniformLocation(GTextProg.GetId(),"translation");
	//if(attribute_coord == -1 || uniform_tex == -1 || uniform_color == -1)
	check();

	glUseProgram(GTextProg.GetId());	check();
	glUniform2f(uniform_scrsize, ScreenWidth, ScreenHeight);

	// Create the vertex buffer object
	glGenBuffers(1, &vbo);

	check();

	/* Create texture atlasses for several font sizes */
	
	atl = new atlas(face, TEXTSIZE);
	check();
}

void LoadTextIdentity() {
	SetTextTranslation(0.0, 0.0);
	SetTextRotation(0.0);
	SetTextColor(255, 255, 255, 255);
}

float ttransx = 0;
float ttransy = 0;
void TranslateText(float x, float y) {
	ttransx += x;
	ttransy += y;
	glUseProgram(GTextProg.GetId());
	glUniform2f(uniform_translation, ttransx, ttransy);
	check();
}
void SetTextTranslation(float x, float y) {
	ttransx = x;
	ttransy = y;
	glUseProgram(GTextProg.GetId());
	glUniform2f(uniform_translation, ttransx, ttransy);
	check();
}

float tangle = 0;
void SetTextRotation(float angle) {
	tangle = angle;
	glUseProgram(GTextProg.GetId());
	glUniform1f(uniform_rotation, tangle);
	check();
}
void RotateText(float angle) {
	tangle += angle;
	glUseProgram(GTextProg.GetId());
	glUniform1f(uniform_rotation, tangle);
	check();
}

short tcr = 0;
short tcg = 0;
short tcb = 0;
short tca = 0;
void SetTextColor(short r, short g, short b, short a) {
	tcr = r;
	tcg = g;
	tcb = b;
	tca = a;
	glUseProgram(GTextProg.GetId());
	glUniform4f(uniform_color, (float)tcr / 255, (float)tcg / 255, (float)tcb / 255, (float)tca / 255);
	check();
}
void SetTextColor(short r, short g, short b) {
	SetTextColor(r, g, b, 255);
}


/**
 * Render text using the currently loaded font and currently set font size.
 * Rendering starts at coordinates (x, y), z is always 0.
 * The pixel coordinates that the FreeType2 library uses are scaled by (sx, sy).
 */
void DrawText(const char *text, float x, float y, float size) {
	const uint8_t *p;

	float sx = size / TEXTSIZE;
	float sy = size / TEXTSIZE;


	glUseProgram(GTextProg.GetId());
	
	glUniform2f(uniform_offset, x, y);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Use the texture containing the atlas */
	glBindTexture(GL_TEXTURE_2D, atl->texture.GetId());
	glUniform1i(uniform_tex, 0);
	
	/* Set up the VBO for our vertex data */
	glEnableVertexAttribArray(attribute_coord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

	point coords[6 * strlen(text)];
	int c = 0;
	
	float lx = 0;
	float ly = 0;
	/* Loop through all characters */
	for (p = (const uint8_t *)text; *p; p++) {
		if (*p == '\n') { 
			lx = 0;
			ly -= size;
			continue;
		}

		// Calculate the vertex and texture coordinates
		float x2 = lx + atl->c[*p].bl * sx;
		float y2 = -ly - atl->c[*p].bt * sy;
		float w = atl->c[*p].bw * sx;
		float h = atl->c[*p].bh * sy;

		// Advance the cursor to the start of the next character
		lx += atl->c[*p].ax * sx;
		ly += atl->c[*p].ay * sy;

		// Skip glyphs that have no pixels
		if (!w || !h)
			continue;

		coords[c++] = (point) {
		x2, -y2, atl->c[*p].tx, atl->c[*p].ty};
		coords[c++] = (point) {
		x2 + w, -y2, atl->c[*p].tx + atl->c[*p].bw / atl->w, atl->c[*p].ty};
		coords[c++] = (point) {
		x2, -y2 - h, atl->c[*p].tx, atl->c[*p].ty + atl->c[*p].bh / atl->h};
		coords[c++] = (point) {
		x2 + w, -y2, atl->c[*p].tx + atl->c[*p].bw / atl->w, atl->c[*p].ty};
		coords[c++] = (point) {
		x2, -y2 - h, atl->c[*p].tx, atl->c[*p].ty + atl->c[*p].bh / atl->h};
		coords[c++] = (point) {
		x2 + w, -y2 - h, atl->c[*p].tx + atl->c[*p].bw / atl->w, atl->c[*p].ty + atl->c[*p].bh / atl->h};
	}

	/* Draw all the character on the screen in one go */
	glBufferData(GL_ARRAY_BUFFER, sizeof coords, coords, GL_DYNAMIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, c);

	glDisableVertexAttribArray(attribute_coord);
	glDisable(GL_BLEND);
	check();
}


void DrawWTextShadowed(const wchar_t *text, float x, float y, float size) {

	float sx = size / TEXTSIZE;
	float sy = size / TEXTSIZE;


	glUseProgram(GTextProg.GetId());
	
	

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Use the texture containing the atlas 
	glBindTexture(GL_TEXTURE_2D, atl->texture.GetId());
	glUniform1i(uniform_tex, 0);
	
	//Set up the VBO for our vertex data 
	glEnableVertexAttribArray(attribute_coord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

	point coords[6 * wcslen(text)];
	int c = 0;

	float lx = 0;
	float ly = 0;

	// Loop through all characters
	for (unsigned int i = 0; i < wcslen(text); i++) {
		const uint16_t p = (const uint16_t)text[i];

		if (p == '\n') {
			lx = 0;
			ly -= size;
			continue;
		}

		// Calculate the vertex and texture coordinates
		float x2 = lx + atl->c[p].bl * sx;
		float y2 = -ly - atl->c[p].bt * sy;
		float w = atl->c[p].bw * sx;
		float h = atl->c[p].bh * sy;

		// Advance the cursor to the start of the next character
		lx += atl->c[p].ax * sx;
		ly += atl->c[p].ay * sy;

		// Skip glyphs that have no pixels
		if (!w || !h)
			continue;

		coords[c++] = (point) {
		x2, -y2, atl->c[p].tx, atl->c[p].ty};
		coords[c++] = (point) {
		x2 + w, -y2, atl->c[p].tx + atl->c[p].bw / atl->w, atl->c[p].ty};
		coords[c++] = (point) {
		x2, -y2 - h, atl->c[p].tx, atl->c[p].ty + atl->c[p].bh / atl->h};
		coords[c++] = (point) {
		x2 + w, -y2, atl->c[p].tx + atl->c[p].bw / atl->w, atl->c[p].ty};
		coords[c++] = (point) {
		x2, -y2 - h, atl->c[p].tx, atl->c[p].ty + atl->c[p].bh / atl->h};
		coords[c++] = (point) {
		x2 + w, -y2 - h, atl->c[p].tx + atl->c[p].bw / atl->w, atl->c[p].ty + atl->c[p].bh / atl->h};
	}

	// Draw all the character on the screen in one go 
	glBufferData(GL_ARRAY_BUFFER, sizeof coords, coords, GL_DYNAMIC_DRAW);

	glUniform4f(uniform_color, 0.0, 0.0, 0.0, 1.0);
	glUniform2f(uniform_offset, x - 1, y - 1);
	glDrawArrays(GL_TRIANGLES, 0, c);

	glUniform4f(uniform_color, (float)tcr / 255, (float)tcg / 255, (float)tcb / 255, (float)tca / 255);
	glUniform2f(uniform_offset, x, y);
	glDrawArrays(GL_TRIANGLES, 0, c);

	glDisableVertexAttribArray(attribute_coord);
	glDisable(GL_BLEND);
	check();
}


void DrawTextShadowed(const char *text, float x, float y, float size) {

	const uint8_t *p;

	float sx = size / TEXTSIZE;
	float sy = size / TEXTSIZE;


	glUseProgram(GTextProg.GetId());
	
	

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Use the texture containing the atlas 
	glBindTexture(GL_TEXTURE_2D, atl->texture.GetId());
	glUniform1i(uniform_tex, 0);
	
	// Set up the VBO for our vertex data 
	glEnableVertexAttribArray(attribute_coord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

	point coords[6 * strlen(text)];
	int c = 0;
	
	float lx = 0;
	float ly = 0;
	// Loop through all characters 
	for (p = (const uint8_t *)text; *p; p++) {

		if (*p == '\n') {
			lx = 0;
			ly -= size;
			continue;
		}

		// Calculate the vertex and texture coordinates
		float x2 = lx + atl->c[*p].bl * sx;
		float y2 = -ly - atl->c[*p].bt * sy;
		float w = atl->c[*p].bw * sx;
		float h = atl->c[*p].bh * sy;

		// Advance the cursor to the start of the next character
		lx += atl->c[*p].ax * sx;
		ly += atl->c[*p].ay * sy;

		// Skip glyphs that have no pixels
		if (!w || !h)
			continue;

		coords[c++] = (point) {
		x2, -y2, atl->c[*p].tx, atl->c[*p].ty};
		coords[c++] = (point) {
		x2 + w, -y2, atl->c[*p].tx + atl->c[*p].bw / atl->w, atl->c[*p].ty};
		coords[c++] = (point) {
		x2, -y2 - h, atl->c[*p].tx, atl->c[*p].ty + atl->c[*p].bh / atl->h};
		coords[c++] = (point) {
		x2 + w, -y2, atl->c[*p].tx + atl->c[*p].bw / atl->w, atl->c[*p].ty};
		coords[c++] = (point) {
		x2, -y2 - h, atl->c[*p].tx, atl->c[*p].ty + atl->c[*p].bh / atl->h};
		coords[c++] = (point) {
		x2 + w, -y2 - h, atl->c[*p].tx + atl->c[*p].bw / atl->w, atl->c[*p].ty + atl->c[*p].bh / atl->h};
	}

	// Draw all the character on the screen in one go //
	glBufferData(GL_ARRAY_BUFFER, sizeof coords, coords, GL_DYNAMIC_DRAW);

	glUniform4f(uniform_color, 0.0, 0.0, 0.0, 1.0);
	glUniform2f(uniform_offset, x - 1, y - 1);
	glDrawArrays(GL_TRIANGLES, 0, c);

	glUniform4f(uniform_color, (float)tcr / 255, (float)tcg / 255, (float)tcb / 255, (float)tca / 255);
	glUniform2f(uniform_offset, x, y);
	glDrawArrays(GL_TRIANGLES, 0, c);

	glDisableVertexAttribArray(attribute_coord);
	glDisable(GL_BLEND);
	check();
}
