#include <U8g2lib.h>
#include <Arduino.h>
#include <adp5350.h>
#include <WiFi.h>
#include <base64.h>

#include "LED.h"
#include "OurServer.h"
#include "USS.h"
#include "IMU.h"
#include "MIC.h"

#pragma region GLOBAL VARIABLES
// variables
const int loop_speed = 100;
const int esp_id = 2;
int state = 0;
unsigned long timer;
unsigned long loop_counter = 0;

String py_file = "turn_on_led.py";

int request_id;
String instruction;
String sensor;
String result = "hello";

// objects
ADP5350 adp;
MPU9255 imu;

OurServer server("iesc-s1.mit.edu", "/608dev/sandbox/adchurch/final/", sensor, instruction, request_id, esp_id);
LED led1(adp, 3, loop_speed);
IMU imu1(imu);
String result1;
MIC mic1(result);


U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI oled(U8G2_R0, 5, 17, 16);

#pragma endregion

void setup() {
    Serial.begin(115200);
    oled.begin();
    oled.setFont(u8g2_font_5x7_tf); //set font on oled
    oled.setCursor(0, 15);
    oled.print("Ready.");
    oled.sendBuffer();
    timer = millis();
}

void loop() {
    mic1.record(4);
    // String result2 = imu1.getGyro(0, "y");
    // String result3 = imu1.getGyro(0, "z");
    // String result4 = imu1.getGyro(0, "all");

    oled.clearBuffer();
    oled.setCursor(0,15);
    Serial.println(result1);
    oled.print(result1.substring(15).c_str());
    // oled.setCursor(0, 25);
    // oled.print(result2.substring(12).c_str());
    // oled.setCursor(0, 35);
    // oled.print(result3.substring(12).c_str());
    // oled.setCursor(0, 45);
    // oled.print(result4.substring(20).c_str());

    oled.sendBuffer();

    while(millis() - timer < 100);
    timer = millis();
}