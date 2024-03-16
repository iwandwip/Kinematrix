/**
 * Flow Meter
 *
 * An Arduino flow meter library that provides calibrated liquid flow and volume measurement with flow sensors.
 *
 * @author sekdiy (https://github.com/sekdiy/FlowMeter)
 * @date 14.07.2015 Initial release.
 * @version See git comments for changes.
 *
 * @todo Split up flow sensor and flow meter into different classes and files.
 */

/*
 *  flowmeter-sens.h
 *
 *  flowmeter sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef FLOWMETER_H
#define FLOWMETER_H

#include "Arduino.h"

/**
 * FlowSensorProperties
 *
 * Structure that holds essential information about a flow sensor.
 * Stores general sensor properties and calibration points.
 *
 * See file G34_Flow_rate_to_frequency.jpg for reference.
 */
typedef struct {
    double capacity;      //!< capacity, upper limit of flow rate (in l/min)
    double kFactor;       //!< "k-factor" (in (pulses/s) / (l/min)), e.g.: 1 pulse/s = kFactor * l/min
    double mFactor[10];   //!< multiplicative correction factor near unity, "meter factor" (per decile of flow)
} FlowSensorProperties;

extern FlowSensorProperties UncalibratedSensor; //!< default sensor
extern FlowSensorProperties FS300A;             //!< see documentation about FS300A/SEN02141B
extern FlowSensorProperties FS400A;             //!< see documentation about FS400A/USN-HS10TA

/**
 * FlowMeter
 */
class FlowMeter {
public:
    /**
     * Initializes a new flow meter object.
     *
     * @param pin  The pin that the flow sensor is connected to (has to be interrupt capable, default: INT0).
     * @param prop The properties of the actual flow sensor being used (default: UncalibratedSensor).
     */
    FlowMeter(unsigned int pin = 2, FlowSensorProperties prop = UncalibratedSensor, void (*callback)(void) = NULL, uint8_t interruptMode = RISING);

    double getCurrentFlowrate();                 //!< Returns the current flow rate since last tick (in l/min).
    double getCurrentVolume();                   //!< Returns the current volume since last tick (in l).

    double getTotalFlowrate();                   //!< Returns the (linear) average flow rate in this flow meter instance (in l/min).
    double getTotalVolume();                     //!< Returns the total volume flown trough this flow meter instance (in l).

    /**
     * The tick method updates all internal calculations at the end of a measurement period.
     *
     * We're calculating flow and volume data over time.
     * The actual pulses have to be sampled using the count method (i.e. via an interrupt service routine).
     *
     * Flow sensor formulae:
     *
     * Let K: pulses per second per unit of measure (i.e. (1/s)/(l/min)),
     *     f: pulse frequency (1/s),
     *     Q: flow rate (l/min),
     *     p: sensor pulses (no dimension/unit),
     *     t: time since last measurements (s).
     *
     * K = f / Q             | units: (1/s) / (l/min) = (1/s) / (l/min)
     * <=>                   | Substitute p / t for f in order to allow for different measurement intervals
     * K = (p / t) / Q       | units: ((1/s)/(l/min)) = (1/s) / (l/min)
     * <=>                   | Solve for Q:
     * Q = (p / t) / K       | units: l/min = 1/s / (1/s / (l/min))
     * <=>                   | Volume in l:
     * V = Q / 60            | units: l = (l/min) / (min)
     *
     * The property K is sometimes stated in pulses per liter or pulses per gallon.
     * In these cases the unit of measure has to be converted accordingly (e.g. from gal/s to l/min).
     * See file G34_Flow_rate_to_frequency.jpg for reference.
     *
     * @param duration The tick duration (in ms).
     */
    void tick(unsigned long duration = 1000);
    void count();                                //!< Increments the internal pulse counter. Serves as an interrupt callback routine.
    void reset();                                //!< Prepares the flow meter for a fresh measurement. Resets all current values, but not the totals.

    /*
     * setters enabling continued metering across power cycles
     */
    FlowMeter *setTotalDuration(unsigned long totalDuration); //!< Sets the total (overall) duration (i.e. after power up).
    FlowMeter *setTotalVolume(double totalVolume);            //!< Sets the total (overall) volume (i.e. after power up).
    FlowMeter *setTotalCorrection(double totalCorrection);    //!< Sets the total (overall) correction factor (i.e. after power up).

    /*
     * convenience methods and calibration helpers
     */
    unsigned int getPin();                       //!< Returns the Arduino pin number that the flow sensor is connected to.

    unsigned long getCurrentDuration();          //!< Returns the duration of the current tick (in ms).
    double getCurrentFrequency();                //!< Returns the pulse rate in the current tick (in 1/s).
    double getCurrentError();                    //!< Returns the error resulting from the current measurement (in %).

    unsigned long getTotalDuration();            //!< Returns the total run time of this flow meter instance (in ms).
    double getTotalError();                      //!< Returns the (linear) average error of this flow meter instance (in %).

protected:
    unsigned int _pin;                           //!< connection pin (has to be interrupt capable!)
    FlowSensorProperties _properties;            //!< sensor properties (including calibration data)
    void (*_interruptCallback)(void);            //!< interrupt callback
    uint8_t _interruptMode;                      //!< interrupt mode (LOW, CHANGE, RISING, FALLING, HIGH)

    unsigned long _currentDuration;              //!< current tick duration (convenience, in ms)
    double _currentFrequency;                    //!< current pulses per second (convenience, in 1/s)
    double _currentFlowrate = 0.0f;              //!< current flow rate (in l/tick), e.g.: 1 l / min = 1 pulse / s / (pulses / s / l / min)
    double _currentVolume = 0.0f;                //!< current volume (in l), e.g.: 1 l = 1 (l / min) / (60 * s)
    double _currentCorrection;                   //!< currently applied correction factor

    unsigned long _totalDuration = 0.0f;         //!< total measured duration since begin of measurement (in ms)
    double _totalVolume = 0.0f;                  //!< total volume since begin of measurement (in l)
    double _totalCorrection = 0.0f;              //!< accumulated correction factors

    volatile unsigned long _currentPulses = 0;   //!< pulses within current sample period
};

/**
 * FlowSensorCalibration
 *
 * Convenience class for manipulating sensor properties.
 */
class FlowSensorCalibration {
public:
    FlowSensorCalibration() {};

    FlowSensorCalibration(FlowSensorProperties properties) : _properties(properties) {};

    FlowSensorCalibration *setProperties(FlowSensorProperties properties) {
        this->_properties = properties;
        return this;
    };

    FlowSensorCalibration *setCapacity(double capacity) {
        this->_properties.capacity = capacity;
        return this;
    }

    FlowSensorCalibration *setKFactor(double kFactor) {
        this->_properties.kFactor = kFactor;
        return this;
    }

    FlowSensorCalibration *setMeterFactorPerDecile(unsigned int decile, unsigned int mFactor) {
        this->_properties.mFactor[decile] = mFactor;
        return this;
    }

    FlowSensorProperties getProperties() {
        return this->_properties;
    }

    double getCapacity() {
        return this->_properties.capacity;
    }

    double getKFactor() {
        return this->_properties.kFactor;
    }

    unsigned int getMeterFactorPerDecile(unsigned int decile) {
        return this->_properties.mFactor[decile];
    }

protected:
    FlowSensorProperties _properties;
};

#endif   // FLOWMETER_H

#ifndef FLOWMETER_SENS_H
#define FLOWMETER_SENS_H

#include "base/sensor-module.h"

#define FLOW_METER_PERIOD 1000

enum flow_meter_index_t {
    FLOW_METER_CURRENT_RATE = 0,
    FLOW_METER_CURRENT_VOLUME = 1,
    FLOW_METER_TOTAL_RATE = 2,
    FLOW_METER_TOTAL_VOLUME = 3,
    FLOW_METER_NUM_VALUE = 4
};

class FlowmeterSens : public BaseSens {
private:
    /*variables*/
    FlowMeter *sensorClass;
    FlowSensorProperties sensorProperties;

    float sensorValue[FLOW_METER_NUM_VALUE];
    uint8_t sensorPin;
    uint32_t sensorTimer;

    /*optional object*/
    void (*sensorCallbackFunc)();

public:
    explicit FlowmeterSens(uint8_t _pin, void (*_callback)(), FlowSensorProperties _properties = UncalibratedSensor);
    ~FlowmeterSens();
    void init() override;
    void update() override;
    void process() override;
    void getValue(float *output) override;
    float getValueFlowMeter(flow_meter_index_t _index = FLOW_METER_CURRENT_RATE) const;
    void setPins(uint8_t _pin);
};

#endif  // FLOWMETER_SENS_H