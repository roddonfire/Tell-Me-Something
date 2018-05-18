//BAT.h
#ifndef BAT_h
#define BAT_h
#include "Arduino.h"
#include <adp5350.h>
class BAT
{
  public:
    BAT(ADP5350 &adp);
    String getVoltage(ADP5350 &adp);
    //String getTimeRemaining();
    //String getTimeUsed();
  private:
    float voltage_from_discharge(float discharge);
    float discharge_from_voltage(float voltage, float error);
    //ADP5350 _adp;
    int _charge_status;
    int _voltage;
};
#endif
