#include "common/draw.h"
#include <math.h>

#include "data.h"

#define PI 3.14159265

#define arraySize(arr) (float)sizeof(arr) / (float)sizeof(arr[0])

extern float FPS;
extern unsigned int GScreenWidth;
extern unsigned int GScreenHeight;

float pxpdeg;

GLfloat horizon[] = {
	 -180.0f, 7.0f,
	 -180.0f, 0.0f,

  	 -70.0f, 0.0f,
	 -70.0f, 7.0f,

	 -180.0f, 0.0f,
	 -30.0f, 0.0f,

	 30.0f, 0.0f,
	 180.0f, 0.0f,

 	 70.0f, 0.0f,
	 70.0f, 7.0f,

	 180.0f, 0.0f,
	 180.0f, 7.0f,
};

GLfloat horizon_up[] = {
	 -70.0f, -7.0f,
	 -70.0f, 0.0f,

	 -70.0f, 0.0f,
	 -30.0f, 0.0f,

	 30.0f, 0.0f,
	 70.0f, 0.0f,

	 70.0f, 0.0f,
	 70.0f, -7.0f,
};

GLfloat horizon_down[] = {
	 -70.0f, 7.0f,
	 -70.0f, 0.0f,

	 -70.0f, 0.0f,
	 -62.0f, 0.0f,

	 -54.0f, 0.0f,
	 -46.0f, 0.0f,

	 -38.0f, 0.0f,
	 -30.0f, 0.0f,

	 30.0f, 0.0f,
	 38.0f, 0.0f,
	 
	 46.0f, 0.0f,
	 54.0f, 0.0f,

	 62.0f, 0.0f,
	 70.0f, 0.0f,

	 70.0f, 0.0f,
	 70.0f, 7.0f,
};

GLfloat compass[50*2*2];


GLfloat speed[50*2*2];

GLfloat speed_border[] = {
	-320.0f, -180.0f,
 	-250.0f, -180.0f,
 	-250.0f, 180.0f,
 	-320.0f, 180.0f,
};

GLfloat altitude_border[] = {
	320.0f, -180.0f,
 	250.0f, -180.0f,
 	250.0f, 180.0f,
 	320.0f, 180.0f,
};

void InitOverlay() {
	pxpdeg = GScreenHeight / 37;

	for (int i = 0; i < 50; i++) {
		compass[i * 4] = i * pxpdeg;
		compass[i * 4 + 1] = 0;
		compass[i * 4 + 2] = i * pxpdeg;
		if (i % 10) {
			compass[i * 4 + 3] = 10;
			
		} else {
			compass[i * 4 + 3] = 20;
		}
		//printf("%.1ff, %.1ff\n%.1ff, %.1ff\n--------------------\n", compass[i * 4], compass[i * 4 + 1], compass[i * 4 + 2], compass[i * 4 + 3]); 
	}

	for (int i = 0; i < 50; i++) {
		speed[i * 4] = 0;
		speed[i * 4 + 1] = i * pxpdeg;

		speed[i * 4 + 2] = -10;
		speed[i * 4 + 3] = i * pxpdeg;

	}
}

void DrawOverlay() {
	
	

	LoadDrawIdentity();
	LoadTextIdentity();

	LineWidth(2.0);
	

	//SetDrawColor(255, 0, 0, 125);
	//DrawRect(0, 0, 50, 50);

	SetDrawColor(0, 255, 0, 255);
	SetTextColor(0, 255, 0, 255);
	//SetDrawColor(255, 255, 255);
	//SetTextColor(255, 255, 255);


	
	Translate(GScreenWidth / 2, GScreenHeight / 2);
	TranslateText(GScreenWidth / 2, GScreenHeight / 2);

	//center
	DrawCircle(0, 0, 8);
	DrawLine(-30, -12, -15, -12);
	DrawLine(-15, -12, -6.5, -6.5);
	DrawLine(15, -12, 6.5, -6.5);
	DrawLine(30, -12, 15, -12);

	DrawPoly(0, 0, GL_LINE_STRIP, speed_border, 4);
	DrawPoly(0, 0, GL_LINE_STRIP, altitude_border, 4);

	SetDrawColor(0, 255, 0, 255);

	//DrawOutlinedRect(-250, 190, 500, 50);// compass
	//DrawOutlinedRect(-320, -180, 70, 360);// speed
	//DrawOutlinedRect(250, -180, 70, 360);// altitude
	//DrawOutlinedRect(-250, -180, 500, 360);//horizon
	

	angle lang = data.ang; // make angle single per frame

	SetClip(-250, 190, 500, 50);// compass

		float x = -(lang.yaw - floor(lang.yaw / 10) * 10 + 20) * pxpdeg;
		DrawPoly(x, 195, GL_LINES, compass, 100);

		for (int i = 0; i < 360; i+= 10) {
			char text[5];

			if (i == 0) {
				sprintf(text, "N");
			} else if (i == 90) {
				sprintf(text, "E");
			} else if (i == 180) {
				sprintf(text, "S");
			} else if (i == 270) {
				sprintf(text, "W");
			} else {
				sprintf(text, "%i", i);
			}
			
			DrawText(text, -lang.yaw * pxpdeg + i * pxpdeg - 6, 220, 18);
		}
	ClearClip();
/*
	SetClip(0, 0, 10, 10);
		glClear(GL_COLOR_BUFFER_BIT);
	ClearClip();*/

	SetClip(-320, -180, 70, 360);// speed
		DrawPoly(-250, -180, GL_LINES, speed, 100);

		//SetClip(-320, -50, 70, 100);// speed

			
	ClearClip();


	SetClip(250, -180, 70, 360);// altitude
		char alt[256];
		sprintf(alt, "%.1f", data.gps.altitude);
		DrawTextShadowed(alt, 255, 0, 24);
	ClearClip();

	Rotate(-lang.roll);
	SetTextRotation(-lang.roll);

	SetClip(-250, -180, 500, 360); //horizon
		DrawPoly(0, -lang.pitch * pxpdeg, GL_LINES, horizon, 12);

		for (int i = -9; i <= 9; i++) {
			float y = (i * 10 + -lang.pitch) * pxpdeg;
			if (i > 0) 
				DrawPoly(0, y, GL_LINES, horizon_up, 8);
			else if (i < 0)
				DrawPoly(0, y, GL_LINES, horizon_down, 16);

			//DrawOutlinedRect(-3, y - 3, 6, 6);
			
			if (i == 0) continue;
			char text[5];
			sprintf(text, "%i", i * 10);
			DrawText(text, 75, y - 6, 18);
			DrawText(text, -100, y - 6, 18);
		}

	ClearClip();

	SetTextRotation(0);


	if (data.gps.status == 'A')
		SetTextColor(0, 255, 0, 255);
	else
		SetTextColor(255, 100, 0, 255);

	char gps[256];
	sprintf(gps, "%.5f%c %.5f%c", data.gps.latitude, data.gps.NS, data.gps.longitude, data.gps.EW);
	DrawTextShadowed(gps, -120, 250, 24);

	
	SetTextTranslation(0, 0);
	SetTextColor(0, 255, 0, 255);

	char fps[256];
	sprintf(fps, "t: %.1f fps: %i", data.mpu.temp, (int)FPS);
	DrawTextShadowed(fps, 16, 12, 24);

	char acc[256];
	sprintf(acc, "p:%.1f \ny:%.1f \nr:%.1f", data.ang.pitch, data.ang.yaw, data.ang.roll);
	DrawTextShadowed(acc, 16, 555, 24);

	//wchar_t wacc[256];
	//swprintf(wacc, 256, L"хуй \nпизда \nебаться \n ☢", data.ang.pitch, data.ang.yaw, data.ang.roll);
	//DrawWTextShadowed(wacc, 16, 200, 24);
}


















/*
int o = 0;

void DrawOverlay() {
	glUseProgram(GDrawProg.GetId());	check();


	glUniform4f(shColor, 0.0, 1.0, 0.0, 1.0);
	glUniform1f(shVertexSize, 1.0);
	glUniform2f(shScale, 1.0, 1.0);
	glUniform2f(shOffset, 0, 0);

	glEnableVertexAttribArray(shVertex);	check();
	glVertexAttribPointer(
		shVertex, //vertexPosition_modelspaceID, // The attribute we want to configure
		2,                  // size
		GL_FLOAT,           // type
		false,           // normalized?
		0,                  // stride
		circle // (void*)0            // array buffer offset
	);	check();

	glUniform2f(shTranslation, GScreenWidth / 2, GScreenHeight / 2);
	glUniform1f(shRotation, 0); // roll
	glUniform2f(shScale, 50.0, 50.0);
	glDrawArrays(GL_LINE_LOOP, 0, 37);


	glUniform2f(shScale, 1.0, 1.0);
	glEnableVertexAttribArray(shVertex);	check();
	glVertexAttribPointer(
		shVertex, //vertexPosition_modelspaceID, // The attribute we want to configure
		2,                  // size
		GL_FLOAT,           // type
		false,           // normalized?
		0,                  // stride
		horizon // (void*)0            // array buffer offset
	);	check();
	
	//glUniform2f(shTranslation, GScreenWidth, GScreenHeight);
	glUniform2f(shTranslation, GScreenWidth / 2, GScreenHeight / 2);

	glUniform1f(shRotation, o); // roll

	glDrawArrays(GL_LINE_STRIP, 0, 7); check();
	



	glUniform2f(shOffset, 0, -50);

	glDrawArrays(GL_LINE_STRIP, 0, 7); check();
	


	// Draw text
	char msg[256];
	
	sprintf(msg, "1oh\nopop%d",o);
	o++;

	SetTextRotation(0.0);
	SetTextColor(0.0, 1.0, 0.0, 1.0);
	DrawTextShadowed(msg, 0, 12, 24);
}

*/
