#ifndef USS_h
#define USS_h
#include "Arduino.h"

class USS {
    long duration = 0;
    float distance = 0;
    const int trigPin = 4;
    const int echoPin = 19;
public:
    USS();
    String getDistance();
    String getAvgDistance(int AvgTime);
};
#endif
