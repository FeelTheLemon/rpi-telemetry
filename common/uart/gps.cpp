#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <unistd.h>

#include "gps.h"

#include "uart.h"

void gpsInit() {
	uart_init("/dev/ttyS0");
	//printf("lol\n");


}

/*
2 $GPRMC,101202.00,A,4840.74305,N,04426.29941,E,0.949,255.50,271216,,,A*62
------------------------------------------
2 $GPVTG,255.50,T,,M,0.949,N,1.758,K,A*35
------------------------------------------
2 $GPGGA,101202.00,4840.74305,N,04426.29941,E,1,04,19.65,100.7,M,1.9,M,,*53
------------------------------------------
2 $GPGSA,A,3,31,26,14,32,,,,,,,,,21.58,19.65,8.92*06
------------------------------------------
2 $GPGSV,4,1,14,01,39,199,,03,83,302,,06,09,326,,09,11,252,*7E
------------------------------------------
2 $GPGSV,4,2,14,11,13,199,,14,27,064,35,17,11,287,,19,12,302,*7C
------------------------------------------
2 $GPGSV,4,3,14,22,79,136,20,23,50,256,,25,10,031,,26,11,115,23*72
------------------------------------------
2 $GPGSV,4,4,14,31,45,063,38,32,02,070,18*7C
------------------------------------------
2 $GPGLL,4840.74305,N,04426.29941,E,101202.00,A,A*67
------------------------------------------

*/
/*
void gpsParseData(gpsData &data, char * buf) {

	char type[3];
	strncpy(type, buf + 3, 3); type[3] = '\0';

	//printf("type %s %i\n", type, strlen(type));

	if(!strcmp(type, "RMC")) {
		printf("%s\n",buf);
		//$GPRMC,101202.00,A,4840.74305,N,04426.29941,E,0.949,255.50,271216,,,A*62
		char ttt[10];
		char ttt2[100];
		char ttt3[100];
		
		int count = sscanf(buf, "$%5s,%9s,%s,",
			ttt,
			ttt2,
			ttt3
		);

		printf("%i ttt %s %s %s\n", count, ttt, ttt2, ttt3);
	}
	if(!strcmp(type, "GLL")) {
		//printf("2 %s",type);
	}	

	//printf("all %s\n", buf);
	//printf("------------------------------------------\n");
}*/

float mintodec(float min) {
	float degfloat = min / 100;
	float deg = floor(degfloat);
	return (deg + (degfloat - deg) * 100 / 60);
}

void gpsParseData(gpsData &data, char * buf) {

	char str[strlen(buf)];
	strcpy(str, buf); str[strlen(buf) - 4] = '\0';//remove checksum

	char type[4];
	strncpy(type, str + 3, 3); type[3] = '\0';

	char * pch = strtok(str, ",");

	std::vector<char*> exp;
	while (pch) {
		exp.push_back(pch);
		pch = strtok(NULL, ",");
	}

	//
	if(!strcmp(type, "RMC")) {
		data.time = atoi(exp[1]); //1 Time
		data.status = exp[2][0];
		if (data.status == 'A') {
			data.latitude = mintodec(atof(exp[3]));
			data.NS = exp[4][0];
			data.longitude = mintodec(atof(exp[5]));
			data.EW = exp[6][0];
			//pch = exp[7]; //speed knots
			data.track = atof(exp[8]);
		}
		
		//data.date = strtok(NULL, ","); 
		//pch = strtok(NULL, ",");
		//char z[15] = {pch[0], pch[1], '.', pch[2], pch[3], '.', pch[4], pch[5], '\0'};
		//strcpy(data.date, z);
	}

	if(!strcmp(type, "VTG")) {
		//printf("2 %s",type);
		if (data.status == 'A') {
			data.speedKph = atof(exp[7]);
		}
	}

	if(!strcmp(type, "GGA")) {
		//printf("2 %s",type);
		if (data.status == 'A') {
			data.satellitesNum = atoi(exp[7]);
			data.altitude = atoi(exp[9]);
			data.gSeparation = atoi(exp[10]);
		}
	}

	if(!strcmp(type, "GSA")) {
		
		if (data.status == 'A') {
			data.PDOP = atof(exp[exp.size() - 3]);
			data.HDOP = atof(exp[exp.size() - 3]);
			data.VDOP = atof(exp[exp.size() - 3]);
		}
	}
}


void gpsUpdateData(gpsData &data) {
	char buffer[1024 * 2];
	unsigned int pos = 0;
	while (1) {
		char buf[512];
		int rx_b = uart_read(buf, sizeof(buf));

		char * pch = strchr(buf, 0x0a);

		//TODO add multiline support

		if (pch == NULL) {
			strcat(buffer, buf);
		} else {
			int loc = pch-buf+1;
			strncat(buffer, buf, loc - 1);
			//strcat(buffer, "\n");

			//sprintf(buffer, "%s", buffer);

			gpsParseData(data, buffer);

			memset(buffer, '\0', sizeof(buffer));
			strncpy( buffer, buf + loc, strlen(buf) - loc);
		}
	}
}

/*
void gpsUpdateData(gpsData &data) {
	char buffer[1024 * 2];
	unsigned int pos = 0;

	while (1) {
		char ret[256];
		int rx_b = uart_read(ret, 1);
		
		if (rx_b < 0){
			printf("no data to read\n");
			usleep(30 * 1000);
			continue;
		}

		switch (ret[0]) {
			case 0x0a:
				buffer[pos] = 0x0a;
				
				gpsParseData(buffer, strlen(buffer));
				memset (buffer, '\0', sizeof(buffer));
				pos = 0;
			break;
			default:
				buffer[pos] = ret[0];
				pos++;
			break;
		}
	}
}*/