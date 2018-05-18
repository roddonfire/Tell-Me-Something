#ifndef IMU_h
#define IMU_h
#include "Arduino.h"
#include "mpu9255_esp32.h"

class IMU
{
    public:
        IMU(MPU9255 &imu);
        String getAccel(int duration, String axis);
        String getGyro(int duration, String axis);
        String getTemp(int duration);
    private:
        MPU9255 _imu;
        // acceleration
        float _x, _y, _z;
        // angular velocity (hence 'a')
        float _ax, _ay, _az;
        float _temp;
};
#endif