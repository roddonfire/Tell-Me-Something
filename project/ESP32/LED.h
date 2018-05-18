#ifndef LED_h
#define LED_h
#include "Arduino.h"
#include "adp5350.h"

class LED
{
    public:
    LED(ADP5350 &adp, int ldo, int loop_speed);
    String set(bool val);
    String flash(bool flashing, float speed);
    void update();
    
    private:
    ADP5350& _adp;
    int _ldo;
    int _loop_speed;
    float _speed;
    int _count;
    bool _flashing;
    bool _status;
};

#endif // !LED_h