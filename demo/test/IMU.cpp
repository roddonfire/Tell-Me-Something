#include "IMU.h"
#include "Arduino.h"
#include "mpu9255_esp32.h"

IMU::IMU(MPU9255 &imu): _imu(imu) {
    if (_imu.readByte(MPU9255_ADDRESS, WHO_AM_I_MPU9255) == 0x73) {
        _imu.initMPU9255();
    }
    else {
        while (1) Serial.println("NOT FOUND"); // Loop forever if communication doesn't happen
    }
    _imu.getAres(); //call this so the IMU internally knows its range/resolution
    _imu.getGres();
    _temp = 0;
    _x = 0;
    _y = 0;
    _z = 0;
    _ax = 0;
    _ay = 0;
    _az = 0;
    _temp = 0;
}

String IMU::getAccel(int duration, String axis) {
    if (duration == 0) { // single measurement
        _imu.readAccelData(_imu.accelCount);
        _x = _imu.accelCount[0] * _imu.aRes;
        _y = _imu.accelCount[1] * _imu.aRes;
        _z = _imu.accelCount[2] * _imu.aRes;
        if (axis == "x") {
            return "Acceleration on x-axis: " + (String)_x;
        }
        else if (axis == "y") {
            return "Acceleration on y-axis: " + (String)_y;
        }
        else if (axis == "z") {
            return "Acceleration on z-axis: " + (String)_z;
            
        }
        else if (axis == "all") {
            return "Acceleration on x, y, and z: " + (String)_x + ", " + (String)_y + ", " + (String)_z;
            
        }
        else {
            return "Axis not recognized";
        }
    }
    else { // average measurement
        int samples = duration * 10;
        float x_r = 0;
        float y_r = 0;
        float z_r = 0; // x result, y result, z result
        long currentTime = millis();
        Serial.println("Measuring...");
        for (int i = 0; i < samples; i++) { // will loop {samples} times
            _imu.readAccelData(_imu.accelCount);
            x_r += _imu.accelCount[0] * _imu.aRes;
            y_r += _imu.accelCount[1] * _imu.aRes;
            z_r += _imu.accelCount[2] * _imu.aRes;
            while (millis() - currentTime < 100); // wait 0.1 seconds
            currentTime = millis();
        }
        x_r /= samples;
        y_r /= samples;
        z_r /= samples;
        if (axis == "x")
            return "Average acceleration on x-axis (over " + (String)(duration) + " seconds): " + (String)x_r;
        else if (axis == "y")
            return "Average acceleration on y-axis: (over " + (String)(duration) + " seconds): " + (String)y_r;
        else if (axis == "z")
            return "Average acceleration on z-axis: (over " + (String)(duration) + " seconds): " + (String)z_r;
        else if (axis == "all")
            return "Average acceleration on x, y, and z: (over " + (String)(duration) + " seconds): " + (String)x_r + ", " + (String)y_r + ", " + (String)z_r;
        else
            return "Axis not recognized";
    }
}

String IMU::getGyro(int duration, String axis) {
    if (duration == 0) { // single measurement
        _imu.readGyroData(_imu.gyroCount);
        _x = _imu.gyroCount[0] * _imu.gRes;
        _y = _imu.gyroCount[1] * _imu.gRes;
        _z = _imu.gyroCount[2] * _imu.gRes;
        if (axis == "x")
            return "Angular speed on x-axis: " + (String)_x;
        else if (axis == "y")
            return "Angular speed on y-axis: " + (String)_y;
        else if (axis == "z")
            return "Angular speed on z-axis: " + (String)_z;
        else if (axis == "all")
            return "Angular speed on x, y, and z: " + (String)_x + ", " + (String)_y + ", " + (String)_z;
        else
            return "Axis not recognized";
    }
    else { // average measurement
        int samples = duration * 10;
        float x_r = 0;
        float y_r = 0;
        float z_r = 0; // x result, y result, z result
        Serial.println("Measuring...");
        long currentTime = millis();
        for (int i = 0; i < samples; i++) { // will loop {samples} times
            _imu.readGyroData(_imu.gyroCount);
            x_r += _imu.gyroCount[0] * _imu.gRes;
            y_r += _imu.gyroCount[1] * _imu.gRes;
            z_r += _imu.gyroCount[2] * _imu.gRes;
            while (millis() - currentTime < 100) ; // wait 0.1 seconds
            currentTime = millis();
        }
        x_r /= samples;
        y_r /= samples;
        z_r /= samples;
        if (axis == "x")
            return "Average angular speed on x-axis (over " + (String)(duration) + " seconds): " + (String)x_r;
        else if (axis == "y")
            return "Average angular speed on y-axis: (over " + (String)(duration) + " seconds): " + (String)y_r;
        else if (axis == "z")
            return "Average angular speed on z-axis: (over " + (String)(duration) + " seconds): " + (String)z_r;
        else if (axis == "all")
            return "Average angular speed on x, y, and z: (over " + (String)(duration) + " seconds): " + (String)x_r + ", " + (String)y_r + ", " + (String)z_r;
        else
            return "Axis not recognized";
    }
}

String IMU::getTemp(int duration) {
    if (duration == 0) { // single measurement
        _temp = _imu.readTempData()/100;
        return "Current temperature: " + (String)_temp;
    }
    else { // average measurement
        float t_r = 0; // temp result
        int samples = duration * 10;
        Serial.println("Measuring...");
        long currentTime = millis();
        for (int i = 0; i < samples; i++) {
            t_r += (float)_imu.readTempData()/100;
            while (millis() - currentTime < 100); // wait 0.1 seconds
            currentTime = millis();
        }
        t_r /= samples;
        return "Average temperature (over " + (String)duration + " seconds): " + (String)t_r;
    }
}