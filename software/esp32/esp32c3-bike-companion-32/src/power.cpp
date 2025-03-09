#include <Arduino.h>
#include <serialutils.h>
#include <power.h>
#include <Wire.h>

Power::Power() {
    XPowersPMU power;
}

void Power::initialize(bool pmu_irq) {
    sout.info() <= "init power";
    Wire.begin(IIC_SDA, IIC_SCL);
    power.begin(Wire, AXP2101_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);

    power.disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
    // power.setChargeTargetVoltage(3);
    // // Clear all interrupt flags
    // power.clearIrqStatus();
    // // Enable the required interrupt function
    // power.enableIRQ(
    //     XPOWERS_AXP2101_PKEY_SHORT_IRQ  //POWER KEY
    // );

    adcOn();
    sout.info() <= "finish init power";
}

void Power::adcOn() {
    power.enableTemperatureMeasure();
    // Enable internal ADC detection
    power.enableBattDetection();
    power.enableVbusVoltageMeasure();
    power.enableBattVoltageMeasure();
    power.enableSystemVoltageMeasure();
}

void Power::adcOff() {
    power.disableTemperatureMeasure();
    // Enable internal ADC detection
    power.disableBattDetection();
    power.disableVbusVoltageMeasure();
    power.disableBattVoltageMeasure();
    power.disableSystemVoltageMeasure();
}

float Power::getTemperature() {
    return power.getTemperature();
}

bool Power::isCharging() {
    return power.isCharging();
}

float Power::getBattVoltage() {
    return power.getBattVoltage();
}

float Power::getBatteryPercent() {
    sout.info() <= "read batt";
    return power.getBatteryPercent();
}
