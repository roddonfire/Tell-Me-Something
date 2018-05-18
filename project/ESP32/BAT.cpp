//BAT.cpp
#include "BAT.h"
#include "Arduino.h"
BAT::BAT(ADP5350 &adp) {
  while (adp.info() != 27){ //set up ADP5350
    delay(1000);
    Serial.print("ADP5350 not connected: ");
    //Serial.println(adp.info());
  }
  adp.setCharger(1); //Turn on charger
  adp.enableFuelGauge(1); //turn on voltage reading
  adp.enableLDO(1, 1); //Turn on LDO1
  adp.enableLDO(2, 1); //Turn on LDO2
  delay(2000);
  adp.voltageLDO3(13); //Set LDO3 to be 3.3V value
  // adp.enableLDO(3,1); //Enable LDO3
  _charge_status = adp.chargerStatus(); //get charger status
  //_adp = adp;
  
}
float BAT::voltage_from_discharge(float discharge){
  float voltage=0;
  voltage = (-5.08321149)*pow(discharge,4)+(8.16962756)*pow(discharge,3)+(-3.53049671)*pow(discharge,2)+(-0.3295403)*discharge+4.08151442;
  return voltage;
}

float BAT::discharge_from_voltage(float voltage, float error){
  float i=0.0;
  float discharge=0.0;
  if (voltage>4.0){
    return discharge;
  }
  while (i<=1.0){
    if (voltage_from_discharge(i)-error<=voltage && voltage_from_discharge(i)+error>=voltage){
      discharge=i;
      return discharge;
    }
    i+=0.001;
  }
}

String BAT::getVoltage(ADP5350 &adp) {
  if (adp.info() != 27) {
    return "Battery Unit(ADP5350) not connected.";
  } else {
    _charge_status = adp.chargerStatus();
    _voltage = adp.batteryVoltage(); // Battery Voltage (mV)
    return "Battery voltage: "+String(0.001*_voltage,5)+"V";
  }
}

//String BAT::getTimeUsed() {
//  if (_adp.info() != 27) {
//    return "Battery Unit(ADP5350) not connected.";
//  } else {
//    //_charge_status = _adp.chargerStatus();
//    _voltage = _adp.batteryVoltage(); // Battery Voltage (mV)
//    float _discharge_amt = discharge_from_voltage(0.001*_voltage, 0.001);
//    return "Battery time used: "+String(9000*_discharge_amt,5)+"sec";
//  }
//}
//
//String BAT::getTimeRemaining() {
//    if (_adp.info() != 27) {
//        return "Battery Unit(ADP5350) not connected.";
//    } else {
//        //_charge_status = _adp.chargerStatus();
//        _voltage = _adp.batteryVoltage(); // Battery Voltage (mV)
//        float _discharge_amt = discharge_from_voltage(0.001*_voltage, 0.001);
//        float time_used=9000*_discharge_amt;
//        float difference=9000-time_used;
//        return "Battery time left: "+String(difference)+"sec";
//    }
//    
//}

//String BAT::getTimeRemaining() {
//    float old_discharge_amt=0;
//    float discharge_amt=0;
//    float discharge_rate=0;
//    float time_remaining=0;
//    unsigned long timer=0;
//    
//    if (_adp.info() != 27) {
//        return "Battery Unit(ADP5350) not connected.";
//    } else {
//        _charge_status = _adp.chargerStatus();
//        _voltage = _adp.batteryVoltage(); // Battery Voltage (mV)
//        old_discharge_amt = discharge_from_voltage(0.001*_voltage, 0.001);
//        if (_charge_status == 0) {
//            timer = millis();
//            while (millis()-timer<10000);
//            _voltage = _adp.batteryVoltage(); // Battery Voltage (mV)
//            discharge_amt = discharge_from_voltage(0.001*_voltage, 0.001);
//            discharge_rate = (discharge_amt-old_discharge_amt)/10.0;
//            //Serial.println("discharge rate: "+String(discharge_rate));
//            time_remaining = (1-discharge_amt)/(discharge_rate);
//        } else {
//            return "Battery is charging";
//        }
//        return "Battery time left: "+String(time_remaining,5)+"sec";
//        //return "DA:"+String(discharge_amt)+"ODA:"+String(old_discharge_amt);
//    }
//}




