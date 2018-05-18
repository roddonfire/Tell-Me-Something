#include "LED.h"
#include "Arduino.h"
#include "adp5350.h"

LED::LED(ADP5350 &adp, int ldo, int loop_speed): _adp(adp) {
    _ldo = ldo;
    _loop_speed = loop_speed;
    _flashing = false;
    _speed = 0;
    _count = 1;
    _status = false;
}

String LED::set(bool val) {
    _adp.enableLDO(_ldo, val);
    _status = val;
    if (_status)
        return "LED turned on";
    else
        return "LED turned off";
}

String LED::flash(bool flashing, float speed) {
    _flashing = flashing;
    _speed = speed;
    if (!_flashing)
        LED::set(false);
    if (_flashing)
        return "LED flashing started";
    else
        return "LED flashing stopped";
}

void LED::update() {
    if (_flashing) { // flashes LED if necessary
        int time_passed = _count * _loop_speed;
        if (time_passed % (int)(_speed * 1000) == 0)
            LED::set(!_status); // flip LED status
    }
    _count++;
}