
#include <vector>

struct satellite {
	unsigned int id;
	float elevation;
	float azimuth;
	float SNR;
};

struct gpsData {
	unsigned int time;
	char status = 'V';
	float latitude;
	char NS = 'N';
	float longitude;
	char EW = 'E';
	float track;
	char date[15];
	float speedKph;
	unsigned short satellitesNum;
	float altitude;
	float gSeparation;
	float PDOP;
	float HDOP;
	float VDOP;
	std::vector<satellite> satellites;
};

void gpsInit();
void gpsUpdateData(gpsData &data);