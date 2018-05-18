#include "SLEEP.h"
#include "Arduino.h"

void sleepNow(int seconds) {
    esp_sleep_enable_timer_wakeup(seconds * 1000000);
    esp_deep_sleep_start();
}
