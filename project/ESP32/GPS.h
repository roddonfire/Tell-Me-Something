#ifndef GPS_h
#define GPS_h
#include "Arduino.h"
#include <adp5350.h>

class GPS {
    int isConnected;
    char satelliteCount[200];
    String lat_from_gps;
    String lon_from_gps;
    String time_from_gps;
    String date_from_gps;
    //ADP5350 adp;
    String data;
    String gpsData[14];

    HardwareSerial gps;
public:
    GPS(HardwareSerial &gps1, ADP5350 &adp1);
    void parseGPS();
    String Connected();
    String getSatelliteCount();
    String getLocation();
    String getTime();
private:
    String removeToComma(String in);
};
#endif
