#ifndef _POWER_H
#define _POWER_H

#include <Arduino.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>

#include "i2c.h"
#include "reset.h"

#define DEFAULT_VREF 1100 // tbd: use adc2_vref_to_gpio() for better estimate
#define NO_OF_SAMPLES 64  // we do some multisampling to get better values

#ifndef BAT_MAX_VOLTAGE
#define BAT_MAX_VOLTAGE 4200 // millivolts
#endif
#ifndef BAT_MIN_VOLTAGE
#define BAT_MIN_VOLTAGE 3100 // millivolts
#endif

typedef uint8_t (*mapFn_t)(uint16_t, uint16_t, uint16_t);

uint16_t read_voltage(void);
void calibrate_voltage(void);
bool batt_sufficient(void);

#ifdef HAS_PMU

#include <axp20x.h>
extern AXP20X_Class pmu;
enum pmu_power_t { pmu_power_on, pmu_power_off, pmu_power_sleep };
void AXP192_powerevent_IRQ(void);
void AXP192_power(pmu_power_t powerlevel);
void AXP192_init(void);
void AXP192_showstatus(void);

#endif // HAS_PMU

// The following map functions were taken from
//
// Battery.h - Battery library
// Copyright (c) 2014 Roberto Lo Giacco
// https://github.com/rlogiacco/BatterySense
 
/**
 * Symmetric sigmoidal approximation
 * https://www.desmos.com/calculator/7m9lu26vpy
 *
 * c - c / (1 + k*x/v)^3
 */
static inline uint8_t sigmoidal(uint16_t voltage, uint16_t minVoltage, uint16_t maxVoltage) {
	// slow
	// uint8_t result = 110 - (110 / (1 + pow(1.468 * (voltage - minVoltage)/(maxVoltage - minVoltage), 6)));

	// steep
	// uint8_t result = 102 - (102 / (1 + pow(1.621 * (voltage - minVoltage)/(maxVoltage - minVoltage), 8.1)));

	// normal
	uint8_t result = 105 - (105 / (1 + pow(1.724 * (voltage - minVoltage)/(maxVoltage - minVoltage), 5.5)));
	return result >= 100 ? 100 : result;
}

/**
 * Asymmetric sigmoidal approximation
 * https://www.desmos.com/calculator/oyhpsu8jnw
 *
 * c - c / [1 + (k*x/v)^4.5]^3
 */
static inline uint8_t asigmoidal(uint16_t voltage, uint16_t minVoltage, uint16_t maxVoltage) {
	uint8_t result = 101 - (101 / pow(1 + pow(1.33 * (voltage - minVoltage)/(maxVoltage - minVoltage) ,4.5), 3));
	return result >= 100 ? 100 : result;
}

/**
 * Linear mapping
 * https://www.desmos.com/calculator/sowyhttjta
 *
 * x * 100 / v
 */
static inline uint8_t linear(uint16_t voltage, uint16_t minVoltage, uint16_t maxVoltage) {
	return (unsigned long)(voltage - minVoltage) * 100 / (maxVoltage - minVoltage);
}

uint8_t read_battlevel(mapFn_t mapFunction = &sigmoidal);

#endif