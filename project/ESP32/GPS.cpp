#include "GPS.h"
#include "Arduino.h"


GPS::GPS(HardwareSerial &gps1, ADP5350 &adp1): gps(gps1) {
    adp1.setCharger(1);
    adp1.enableFuelGauge(1);
    adp1.enableLDO(1, 1);
    adp1.enableLDO(2, 1);
    delay(1000);
    adp1.enableLDO(3, 1);
    Serial.println("Enabling LDO3 for GPS");
    gps.begin(9600,SERIAL_8N1,32,33);
    //gps=&gps1;
    //adp=adp1;
    isConnected=0;
    satelliteCount[0]='\0';
}


String GPS::Connected() {
    isConnected=0;
    parseGPS();
    //delay(1000);
    if (isConnected) {
        return "Satellite connection is good";
    }
    else{
        return "Unable to connect to satellite";
    }
}

String GPS::getSatelliteCount() {
    satelliteCount[0] = '\0';
    parseGPS();
    if (isConnected) {
        return "Number of Satellites: "+String(satelliteCount);
    } else {
        return "Unable to connect to satellite";
    }
}

String GPS::getLocation() {
    lat_from_gps[0] = '\0';
    lon_from_gps[0] = '\0';
    parseGPS();
    if (isConnected) {
        return "Location: "+String(lat_from_gps)+", "+String(lon_from_gps);
    } else {
        return "Unable to connect to satellite";
    }
}

String GPS::getTime() {
    time_from_gps[0]='\0';
    parseGPS();
    if (isConnected) {
        return "UTC Time: "+String(time_from_gps);
    } else {
        return "Unable to connect to satellite";
    }
}

void GPS::parseGPS(){
    isConnected=0;
    satelliteCount[0]='\0';
    unsigned long timer = millis();
    for (int i=0; i<100 && isConnected==0; i++) {
        if (gps.available()) {     // If anything comes in Serial1 (pins 0 & 1)
            data = gps.readStringUntil('\n'); // read it and send it out Serial (USB)
        }
        if (data.length()>0){
            Serial.println(data);
        }
        else {
            Serial.print("Data length is 0. ");
        }
        
        //skip to first two commas
        int comma = data.indexOf(",");
        int nextcomma = data.indexOf(",",comma+5);
        if (data.substring(1,6).equals("GPGSV")){
            int second_comma = data.indexOf(",",comma+1);
            //skip to fourth and fifth comma
            int third_comma = data.indexOf(",",second_comma+1);
            int fourth_comma = data.indexOf(",",third_comma+1);
            data.substring(third_comma+1,fourth_comma).toCharArray(satelliteCount, 200);
            
        }
        if(data.substring(1,6).equals("GNRMC")){
            if(data.length() > 40){
                // old code
                Serial.println("in block");
                isConnected=1;

                // new code
                int i = 0;
                int last = 0;
                while (data.indexOf(',') != -1)
                {
                    gpsData[i] = data.substring(0, data.indexOf(','));
                    data = removeToComma(data);
                    // debug
                    // Serial.print("Element " + String(i) + ": " + gpsData[i] + "; ");
                    i++;
                    last = i;
                }
                gpsData[last] = data;
                int index = 0;

                // parse longitude at index 3
                index = gpsData[3].indexOf('.');
                lon_from_gps = gpsData[3].substring(index - 4, index - 2) + String(" degrees, ");
                lon_from_gps += gpsData[3].substring(index - 2, index);
                lon_from_gps += gpsData[3].substring(index) + String("\' ");
                lon_from_gps += gpsData[4];

                // parse latitude at index 5
                index = gpsData[5].indexOf('.');
                lat_from_gps = gpsData[5].substring(index - 4, index - 2) + String(" degrees, ");
                lat_from_gps += gpsData[5].substring(index - 2, index);
                lat_from_gps += gpsData[5].substring(index) + String("\' ");
                lat_from_gps += gpsData[6];

                // parse time at index 1
                time_from_gps = gpsData[1].substring(0, 2) + String(":");
                time_from_gps += gpsData[1].substring(2, 4) + String(":");
                time_from_gps += gpsData[1].substring(4, 6);

                // parse date at index 9; has space at enableLDO
                date_from_gps = gpsData[9].substring(2, 4) + String("/");
                date_from_gps += gpsData[9].substring(0, 2) + String("/");
                date_from_gps += gpsData[9].substring(4, 6) + String(" ");

                Serial.println("Longitude: " + lon_from_gps);
                Serial.println("Latitude: " + lat_from_gps);
                Serial.println("Time: " + time_from_gps);
                Serial.println("Date: " + date_from_gps);
            }
        }
        while (millis() - timer < 50);
        timer = millis();
    }
}

String GPS::removeToComma(String in)
{
    in = in.substring(in.indexOf(',') + 1);
    return in;
}
