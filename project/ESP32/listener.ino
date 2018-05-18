#include <Arduino.h>
#include <adp5350.h>
#include <WiFi.h>
#include <base64.h>
#include <EEPROM.h>
#define EEPROM_SIZE 64

#include "LED.h"
#include "OurServer.h"
#include "USS.h"
#include "IMU.h"
#include "MIC.h"
#include "GPS.h"
#include "SLEEP.h"
#include "BAT.h"

#pragma region GLOBAL VARIABLES
// variables
const int loop_speed = 100; // in milliseconds
const int esp_id = 2; // different for every esp, allows individual instructions
int state = 0;
// 0 = full power; 1 = wifi only on every 30 seconds; 2 = deep sleep for 10 minutes
int power_state = 0;
bool check_server = true;
unsigned long timer;
unsigned long loop_counter = 0; // increments every loop
unsigned long sleep_counter = 0;
// loop_speed*loop_counter gives approximate time since last instruction 
// (will typically be an underestimate, due to blocking average functions)

String py_file = "database.py";

int request_id;
String instruction;
String sensor;
String result;
String mic_result;

// objects
ADP5350 adp;
MPU9255 imu;
HardwareSerial gps(2);

OurServer server("iesc-s1.mit.edu", "/608dev/sandbox/adchurch/final/", sensor, instruction, request_id, esp_id);
// for power testing purposes
OurServer power_reporter("iesc-s1.mit.edu", "/608dev/sandbox/adchurch/final/", sensor, instruction, request_id, esp_id);
    LED led1(adp, 3, loop_speed);
//IMU imu1(imu);
MIC mic1(mic_result);
//BAT bat1(adp);
USS uss1;
GPS gps1(gps, adp);

#pragma endregion

#pragma region HELPER METHODS

void split_str(String in, char split, String* out) {
    int i = 0;
    while (in.indexOf(split) != -1) {
        out[i] = in.substring(0, in.indexOf(split));
        in = in.substring(in.indexOf(split) + 1);
        i++;
    }
    out[i] = in;
    Serial.println("Split string array: {" + out[0] + ", " + out[1] + ", " + out[2] + "}" );
}

String parse_LED() {
    String instr[3] = {};
    split_str(instruction, '$', instr);
    String category = instr[0];
    if (category == "set") { // turn LED on/off
        led1.flash(false, 0);
        return led1.set(instr[1].toInt());
    }
    else if (category == "flash") { // flash LED
        if (instr[1].toInt() == 1) { // start flashing
            return led1.flash(true, instr[2].toInt());
        }
        else { // stop flashing
            return led1.flash(false, 0);
        }
    }
    else {
        return "Error: unrecognized command";
    }

}

String parse_IMU()
{
    IMU imu1(imu);
    String instr[3] = {};
    split_str(instruction, '$', instr);
    String category = instr[0];
    String request = instr[1];
    String params = instr[2];
    if (category == "accel")
    {
        if (request == "single")
        {
            return imu1.getAccel(0, params);
        }
        else if (request == "avg")
        {
            int i = params.indexOf(',');
            String axis = params.substring(0, i);
            int duration = params.substring(i + 1).toInt();
            return imu1.getAccel(duration, axis);
        }
    }
    else if (category == "gyro")
    {
        if (request == "single")
        {
            return imu1.getGyro(0, params);
        }
        else if (request == "avg")
        {
            int i = params.indexOf(',');
            String axis = params.substring(0, i);
            int duration = params.substring(i + 1).toInt();
            Serial.println("Axis: " + axis);
            return imu1.getGyro(duration, axis);
        }
    }
    else if (category == "temp")
    {
        if (request == "single")
        {
            return imu1.getTemp(0);
        }
        else if (request == "avg")
        {
            return imu1.getTemp(params.toInt());
        }
    }

    return "Error: unrecognized command";
}

String parse_BAT()
{
    BAT bat1(adp);
    String instr[3] = {};
    split_str(instruction, '$', instr);
    String category = instr[0];
    if (category == "volt")
    {
        String bat_result = bat1.getVoltage(adp);
        Serial.println("BAT Result: " + bat_result.substring(0, 30));
        return bat_result;
    }
    return "Error: unrecognized command";
}

String parse_GPS()
{
    String instr[3] = {};
    split_str(instruction, '$', instr);
    String category = instr[0];
    String req = instr[1];
    if (category == "status")
    {
        if (req == "con")
        {
            String gps_result = gps1.Connected();
            Serial.println("GPS Connection: " + String(gps_result));
            return gps_result;
        }
        else if (req == "sat")
        {
            String gps_result = gps1.getSatelliteCount();
            Serial.println("GPS Satelites: " + String(gps_result));
            return gps_result;
        }
        else
        {
            return "Invalid request";
        }
    }
    else if (category == "loc")
    {
        String gps_result = gps1.getLocation();
        Serial.println("GPS Location: " + String(gps_result));
        return gps_result;
    }
    else if (category == "time")
    {
        String gps_result = gps1.getTime();
        Serial.println("GPS Time: " + String(gps_result));
        return gps_result;
    }
    else
    {
        return "Error: unrecognized command";
    }
}

String parse_MIC()
{
    String instr[3] = {};
    split_str(instruction, '$', instr);
    String category = instr[0];
    int duration = instr[1].toInt();
    if (category == "rec")
    {
        if (duration < 0 || duration > 5)
        {
            return "Invalid duration; please give a duration between 0 and 5 seconds";
        }
        else
        {
            mic1.record(duration);
            Serial.println("Mic Result: " + mic_result.substring(0, 30));
            Serial.println("End of Mic Result: " + mic_result.substring(mic_result.length() - 40));
            server.post_result(py_file, mic_result);
            return "";
        }
    }

    return "Error: unrecognized command";
}

String parse_USS()
{
    String instr[3] = {};
    split_str(instruction, '$', instr);
    String category = instr[0];
    if (category == "avg")
    {
        int duration = instr[1].toInt();
        if (duration < 0 || duration > 600)
        {
            return "Invalid duration; please give a duration between 0 and 600 seconds";
        }
        else
        {
            String uss_result = uss1.getAvgDistance(duration);
            Serial.println("USS Avg Distance: " + String(uss_result));
            return uss_result;
        }
    }
    else if (category == "single")
    {
        String uss_result = uss1.getDistance();
        Serial.println("USS Distance: " + String(uss_result));
        return uss_result;
    }
    else
    {
        return "Error: unrecognized command";
    }
}

String parse_OFF()
{
    String instr[3] = {};
    split_str(instruction, '$', instr);
    String category = instr[0];
    if (category == "sleep")
    {
        int duration = instr[1].toInt();
        if (duration < 0 || duration > 1000)
        {
            return "Invalid duration; please give a duration between 0 and 1000 minutes";
        }
        else
        {
            Serial.println("Sleeping...");
            led1.set(0);
            EEPROM.write(0, byte(0));
            EEPROM.commit();
            if (duration == 1) 
                server.post_result(py_file, "ESP sleeping for " + (String)duration + " minute.");
            else
                server.post_result(py_file, "ESP sleeping for " + (String)duration + " minutes.");

            sleepNow(duration*60); // On wake up, will start with setup()
            return "";
        }
    }
    return "Error: unrecognized command";
}

#pragma endregion

#pragma region FLOW METHODS
void setup() {
    Serial.begin(115200);
    Wire.begin(); //begin i2c comms

    delay(1000);

    if (!EEPROM.begin(EEPROM_SIZE))
    {
        Serial.println("failed to initialise EEPROM");
        delay(1000000);
    }


    Serial.print("EEPROM last state: ");
    power_state = (int)byte(EEPROM.read(0));
    Serial.print(power_state);

    // Write 0 to make sure the ESP will not go back into low power mode if it is reset for other reasons
    EEPROM.write(0, byte(0));
    EEPROM.commit();

    server.start_wifi("6s08", "iesc6s08");

    mic_result.reserve(100000);

    timer = millis();
}

void loop() {

    // if (loop_counter % 200 == 0 || power_state == 2) {
    //     Serial.println("\n\n=============================");
    //     Serial.println("Debugging GET request \n \n");
    //     power_reporter.get_instructions("power.py");
    //     Serial.println("============================= \n\n");
    // }


    if (check_server) { // full power mode
        // state machine
        // the logic for parsing instructions from the server is here
        // (although often it will be better to put it out to a helper method to avoid super long code)
        // the actual actions by the sensors should be handled by their libraries, not code here.
        switch(state) {
            case 0: { // listening
                // only performs GET request every 30 cycles (3 seconds)
                if (loop_counter % 30 != 0)
                    break;
                else if (server.get_instructions(py_file)) {
                    loop_counter = 0;
                    check_server = true;
                    power_state = 0;
                    if (sensor == "IMU")
                        state = 2;
                    else if (sensor == "MIC")
                        state = 3;
                    else if (sensor == "GPS")
                        state = 4;
                    else if (sensor == "USS")
                        state = 5;
                    else if (sensor == "LED")
                        state = 6;
                    else if (sensor == "BAT")
                        state = 7;
                    else if (sensor == "OFF")
                        state = 1;
                }
                else if (power_state == 1)
                {
                    Serial.println("Staying in state 1");
                    loop_counter = 0;
                }
                else if (loop_counter * loop_speed > 300000)
                { // 5 minutes, should be 300000; 10000 to debug
                    power_state = 1;
                    Serial.println("New power state: 1");
                    loop_counter = 0;
                    sleep_counter = 0;
                }

                break;
            }
            case 1: { // power saving
                Serial.println("Switched to state 1");
                parse_OFF();
                break;
            }
            case 2: { // IMU
                Serial.println("Switched to state 2");
                result = parse_IMU();
                state = 8;
                break;
            }
            case 3: { // MIC
                Serial.println("Switched to state 3");
                result = parse_MIC();
                state = 0;
                break;
            }
            case 4: { // GPS
                Serial.println("Switched to state 4");
                result = parse_GPS();
                state = 8;
                break;
            }
            case 5: { // USS
                Serial.println("Switched to state 5");
                result = parse_USS();
                state = 8;
                break;
            }
            case 6: {// LED
                Serial.println("Switched to state 6");
                result = parse_LED();
                state = 8;
                break;
            }
            case 7: { // BAT
                Serial.println("Switched to state 7");
                result = parse_BAT();
                state = 8;
                break;
            }
            case 8: { // Send to server
                server.post_result(py_file, result);
                state = 0;
                break;
            }
        }
    }

    if (power_state == 1)
    { // full ESP power, WiFi only on in bursts
        if (sleep_counter * loop_speed > 3600000) // should be 3600000; 30000 to debug
        { // 60 minutes, no action
            power_state = 2;
        }
        else if (loop_counter * loop_speed > 30000) // should be 30000; 10000 to debug
        { // 30 seconds
            Serial.println("Checking WiFi...");
            server.set_wifi_mode(true);
            check_server = true;
        }
        else
        {
            server.set_wifi_mode(false);
            check_server = false;
            if (loop_counter % 100 == 0) {
                Serial.println("WiFi off, saving power");
            }
        }
    }

    else if (power_state == 2)
    {
        EEPROM.write(0, byte(2));
        EEPROM.commit();

        Serial.println("Sleeping for 10 minutes");
        sleepNow(600); // sleep for 10 minutes; should be 600, 10 to debug
    }

    // update any interloop actions (e.g. flashing led)
    led1.update();

    // end of loop actions
    loop_counter++;
    sleep_counter++;
    while (millis() - timer < loop_speed);
    timer = millis();
}
