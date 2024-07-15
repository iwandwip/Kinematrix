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
#ifdef USE_FLOWMETER_SENS
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
    FlowMeter(unsigned int pin = 2, FlowSensorProperties prop = UncalibratedSensor, void (*callback)(void) = NULL,
              uint8_t interruptMode = RISING);

    double getCurrentFlowrate();                 //!< Returns the current flow rate since last tick (in l/min).
    double getCurrentVolume();                   //!< Returns the current volume since last tick (in l).

    double
    getTotalFlowrate();                   //!< Returns the (linear) average flow rate in this flow meter instance (in l/min).
    double
    getTotalVolume();                     //!< Returns the total volume flown trough this flow meter instance (in l).

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
    void
    count();                                //!< Increments the internal pulse counter. Serves as an interrupt callback routine.
    void
    reset();                                //!< Prepares the flow meter for a fresh measurement. Resets all current values, but not the totals.

    /*
     * setters enabling continued metering across power cycles
     */
    FlowMeter *
    setTotalDuration(unsigned long totalDuration); //!< Sets the total (overall) duration (i.e. after power up).
    FlowMeter *
    setTotalVolume(double totalVolume);            //!< Sets the total (overall) volume (i.e. after power up).
    FlowMeter *setTotalCorrection(
            double totalCorrection);    //!< Sets the total (overall) correction factor (i.e. after power up).

    /*
     * convenience methods and calibration helpers
     */
    unsigned int
    getPin();                       //!< Returns the Arduino pin number that the flow sensor is connected to.

    unsigned long getCurrentDuration();          //!< Returns the duration of the current tick (in ms).
    double getCurrentFrequency();                //!< Returns the pulse rate in the current tick (in 1/s).
    double getCurrentError();                    //!< Returns the error resulting from the current measurement (in %).

    unsigned long getTotalDuration();            //!< Returns the total run time of this flow meter instance (in ms).
    double
    getTotalError();                      //!< Returns the (linear) average error of this flow meter instance (in %).

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

class FlowmeterSens : public BaseSens {
private:
    JsonDocument *doc;
    const char *name;

    FlowMeter *sensorClass;
    uint32_t sensorTimer;

public:
    explicit FlowmeterSens(uint8_t _pin, void (*_callback)(), FlowSensorProperties _properties = UncalibratedSensor);
    ~FlowmeterSens();

    bool init() override;
    bool update() override;
    void process() override;

    void resetValue();

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    void setTotalVolume(double totalVolume);
    void reset(void (*resetCallback)() = nullptr);
};




FlowMeter::FlowMeter(unsigned int pin, FlowSensorProperties prop, void (*callback)(void), uint8_t interruptMode) :
        _pin(pin),                                                              //!< store pin number
        _properties(prop),                                                      //!< store sensor properties
        _interruptCallback(callback),
        _interruptMode(interruptMode) {
    pinMode(this->_pin,
            INPUT_PULLUP);                                      //!< initialize interrupt pin as input with pullup

    if (this->_interruptCallback !=
        NULL) {                                 //!< if ISR callback has been provided, attach it
        attachInterrupt(this->_pin, this->_interruptCallback, this->_interruptMode);
    }

    this->reset();                                                          //!< ignore pulses generated during initialisation
}

double FlowMeter::getCurrentFlowrate() {
    return this->_currentFlowrate;                                          //!< in l/min
}

double FlowMeter::getCurrentVolume() {
    return this->_currentVolume;                                            //!< in l
}

double FlowMeter::getTotalFlowrate() {
    return this->_totalVolume / (this->_totalDuration / 1000.0f) * 60.0f;   //!< in l/min
}

double FlowMeter::getTotalVolume() {
    return this->_totalVolume;                                              //!< in l
}

void FlowMeter::tick(unsigned long duration) {
    /* sampling */
    noInterrupts();                                                         //!< going to change interrupt variable(s)
    volatile unsigned long pulses = this->_currentPulses;                   //!< sample current pulses from counter
    this->_currentPulses = 0;                                               //!< reset pulse counter after successful sampling
    interrupts();                                                           //!< done changing interrupt variable(s)

    /* normalisation */
    double seconds =
            duration / 1000.0f;                                    //!< normalised duration (in s, i.e. per 1000ms)
    double frequency = pulses / seconds;                                    //!< normalised frequency (in 1/s)

    /* determine current correction factor (from sensor properties) */
    unsigned int decile = floor(10.0f * frequency / (this->_properties.capacity *
                                                     this->_properties.kFactor));          //!< decile of current flow relative to sensor capacity
    unsigned int ceiling = 9;                                                                                          //!< highest possible decile index
    this->_currentCorrection = this->_properties.kFactor / this->_properties.mFactor[min(decile,
                                                                                         ceiling)];             //!< combine constant k-factor and m-factor for decile

    /* update current calculations: */
    this->_currentFlowrate = frequency /
                             this->_currentCorrection;          //!< get flow rate (in l/min) from normalised frequency and combined correction factor
    this->_currentVolume = this->_currentFlowrate / (60.0f /
                                                     seconds);        //!< get volume (in l) from normalised flow rate and normalised time

    /* update statistics: */
    this->_currentDuration = duration;                                      //!< store current tick duration (convenience, in ms)
    this->_currentFrequency = frequency;                                    //!< store current pulses per second (convenience, in 1/s)
    this->_totalDuration += duration;                                       //!< accumulate total duration (in ms)
    this->_totalVolume += this->_currentVolume;                             //!< accumulate total volume (in l)
    this->_totalCorrection += this->_currentCorrection * duration;          //!< accumulate corrections over time
}

void FlowMeter::count() {
    this->_currentPulses++;                                                 //!< this should be called from an interrupt service routine
}

void FlowMeter::reset() {
    noInterrupts();                                                         //!< going to change interrupt variable(s)
    this->_currentPulses = 0;                                               //!< reset pulse counter
    interrupts();                                                           //!< done changing interrupt variable(s)

    this->_currentFrequency = 0.0f;
    this->_currentDuration = 0.0f;
    this->_currentFlowrate = 0.0f;
    this->_currentVolume = 0.0f;
    this->_currentCorrection = 0.0f;
}

unsigned int FlowMeter::getPin() {
    return this->_pin;
}

unsigned long FlowMeter::getCurrentDuration() {
    return this->_currentDuration;                                          //!< in ms
}

double FlowMeter::getCurrentFrequency() {
    return this->_currentFrequency;                                         //!< in 1/s
}

double FlowMeter::getCurrentError() {
    /// error (in %) = error * 100
    /// error = correction rate - 1
    /// correction rate = k-factor / correction
    return (this->_properties.kFactor / this->_currentCorrection - 1) * 100;  //!< in %
}

unsigned long FlowMeter::getTotalDuration() {
    return this->_totalDuration;                                            //!< in ms
}

double FlowMeter::getTotalError() {
    /// average error (in %) = average error * 100
    /// average error = average correction rate - 1
    /// average correction rate = k-factor / corrections over time * total time
    return (this->_properties.kFactor / this->_totalCorrection * this->_totalDuration - 1) * 100;
}

FlowMeter *FlowMeter::setTotalDuration(unsigned long totalDuration) {
    this->_totalDuration = totalDuration;
    return this;
}

FlowMeter *FlowMeter::setTotalVolume(double totalVolume) {
    this->_totalVolume = totalVolume;
    return this;
}

FlowMeter *FlowMeter::setTotalCorrection(double totalCorrection) {
    this->_totalCorrection = totalCorrection;
    return this;
}

FlowSensorProperties UncalibratedSensor = {60.0f, 5.0f, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
FlowSensorProperties FS300A = {60.0f, 5.5f, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
FlowSensorProperties FS400A = {60.0f, 4.8f, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};


FlowmeterSens::FlowmeterSens(uint8_t _pin, void (*_callback)(), FlowSensorProperties _properties)
        : sensorTimer(0) {
    sensorClass = new FlowMeter(digitalPinToInterrupt(_pin), _properties, _callback, RISING);
}

FlowmeterSens::~FlowmeterSens() = default;

bool FlowmeterSens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "FlowmeterSens";
        doc = new JsonDocument;
    }
    (*doc)[name]["currentRate"] = 0;
    (*doc)[name]["currentVolume"] = 0;
    (*doc)[name]["totalRate"] = 0;
    (*doc)[name]["totalVolume"] = 0;
    return true;
}

bool FlowmeterSens::update() {
    if (millis() - sensorTimer >= 1000) {
        sensorClass->tick(1000);
        (*doc)[name]["currentRate"] = (float) sensorClass->getCurrentFlowrate();
        (*doc)[name]["currentVolume"] = (float) sensorClass->getCurrentVolume();
        (*doc)[name]["totalRate"] = (float) sensorClass->getTotalFlowrate();
        (*doc)[name]["totalVolume"] = (float) sensorClass->getTotalVolume();
        sensorTimer = millis();
        return true;
    }
    return false;
}

void FlowmeterSens::process() {
    sensorClass->count();
}

void FlowmeterSens::resetValue() {
    (*doc)[name]["currentRate"] = 0;
    (*doc)[name]["currentVolume"] = 0;
    (*doc)[name]["totalRate"] = 0;
    (*doc)[name]["totalVolume"] = 0;
    sensorClass->setTotalVolume(0);
    sensorClass->reset();
}

void FlowmeterSens::setDocument(const char *objName) {
    name = objName;
}

void FlowmeterSens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument FlowmeterSens::getDocument() {
    return (*doc);
}

JsonVariant FlowmeterSens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

void FlowmeterSens::setTotalVolume(double totalVolume) {
    sensorClass->setTotalVolume(totalVolume);
}

void FlowmeterSens::reset(void (*resetCallback)()) {
    sensorClass->reset();
    if (resetCallback != nullptr) resetCallback();
}


#endif  // FLOWMETER_SENS_H
#endif