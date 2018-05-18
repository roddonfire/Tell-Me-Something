#include "USS.h"
#include "Arduino.h"

    USS::USS() {
        pinMode(trigPin, OUTPUT);
        pinMode(echoPin, INPUT);
    }

    String USS::getDistance() {
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        // Sets the trigPin on HIGH state for 10 micro seconds
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        // Reads the echoPin, returns the sound wave travel time in microseconds
        duration = pulseIn(echoPin, HIGH);
        // Calculating the distance
        distance= duration*0.034/2;
        // Prints the distance on the Serial Monitor
        Serial.print("Distance: ");
        return("Distance: " + (String)distance);
    }

    String USS::getAvgDistance(int AvgTime) {
        int samples = 10*AvgTime;
        distance = 0;
        long currentTime=millis();
        for (int i=0; i < samples; i++) {
            digitalWrite(trigPin, LOW);
            delayMicroseconds(2);
            // Sets the trigPin on HIGH state for 10 micro seconds
            digitalWrite(trigPin, HIGH);
            delayMicroseconds(10);
            digitalWrite(trigPin, LOW);
            // Reads the echoPin, returns the sound wave travel time in microseconds
            duration = pulseIn(echoPin, HIGH);
            //Serial.println("Duration"+String(duration));
            // Calculating the distance
            distance += (duration*0.034/2.0);
            //Serial.println("Distance="+String(distance));
            //Serial.print("Distance: ");
            while (millis() - currentTime < 100); // wait 0.1 seconds
            currentTime = millis();
        }
        distance = distance*(1.0/samples);
        return("Avg Distance(cm): " + (String)distance);
    }
