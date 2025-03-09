#ifndef _POWER_H
#define _POWER_H

#include <Arduino.h>
#include <globalconfig.h>
#include "pin_config.h"
#include "XPowersLib.h"

class Power {

private:
    float _batt_voltage;
    bool _pmu_flag = false;
    XPowersPMU power;

public:
    Power();

    void initialize(bool pmu_irq);
    void adcOn();
    void adcOff();

    float getTemperature();
    bool isCharging();
    float getBattVoltage();
    int8_t getBatteryPercent();
};

#endif
