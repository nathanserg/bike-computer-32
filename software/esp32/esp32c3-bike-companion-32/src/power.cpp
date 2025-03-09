#include <Arduino.h>
#include <serialutils.h>
#include <power.h>
#include <Wire.h>

Power::Power() {}

void Power::initialize(bool pmu_irq) {
    sout.info() <= "init power";
    Wire.begin(IIC_SDA, IIC_SCL);
    if (!power.begin(Wire, AXP2101_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
        sout.err() <= "PMU not found!";
        return;
    }

    power.disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
    power.setChargeTargetVoltage(3);
    // Clear all interrupt flags
    power.clearIrqStatus();
    // Enable the required interrupt function
    power.enableIRQ(
        XPOWERS_AXP2101_PKEY_SHORT_IRQ  //POWER KEY
    );

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
    return power.getBattVoltage() / 1000.0;
}

int8_t Power::getBatteryPercent() {
    return int(power.getBatteryPercent());
}
