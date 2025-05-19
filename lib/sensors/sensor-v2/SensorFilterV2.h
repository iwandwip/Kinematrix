#ifndef SENSOR_FILTER_V2_H
#define SENSOR_FILTER_V2_H

#include "Arduino.h"

enum FilterType {
    FILTER_NONE,
    FILTER_MOVING_AVERAGE,
    FILTER_MEDIAN,
    FILTER_KALMAN,
    FILTER_EXPONENTIAL
};

struct FilterParams {
    union {
        struct {
            uint8_t windowSize;
        } movingAverage;

        struct {
            uint8_t windowSize;
        } median;

        struct {
            float processNoise;
            float measurementNoise;
            float estimateError;
        } kalman;

        struct {
            float alpha;
        } exponential;
    };
};

class SensorModuleV2;

class BaseSensV2;

class BaseFilter {
public:
    BaseFilter();
    virtual ~BaseFilter();

    virtual float filter(float newValue) = 0;
    virtual void reset();
    virtual FilterType getType() const = 0;
};

class MovingAverageFilter : public BaseFilter {
private:
    float *_buffer;
    uint8_t _windowSize;
    uint8_t _currentIndex;
    uint8_t _count;
    float _sum;

public:
    MovingAverageFilter(uint8_t windowSize);
    ~MovingAverageFilter();

    float filter(float newValue) override;
    void reset() override;
    FilterType getType() const override;
};

class MedianFilter : public BaseFilter {
private:
    float *_buffer;
    float *_sortedBuffer;
    uint8_t _windowSize;
    uint8_t _currentIndex;
    uint8_t _count;

    void sortBuffer();

public:
    MedianFilter(uint8_t windowSize);
    ~MedianFilter();

    float filter(float newValue) override;
    void reset() override;
    FilterType getType() const override;
};

class KalmanFilter : public BaseFilter {
private:
    float _processNoise;
    float _measurementNoise;
    float _estimateError;
    float _currentEstimate;
    float _lastEstimate;
    float _kalmanGain;
    bool _initialized;

public:
    KalmanFilter(float processNoise, float measurementNoise, float estimateError);
    ~KalmanFilter();

    float filter(float newValue) override;
    void reset() override;
    FilterType getType() const override;

    void setParameters(float processNoise, float measurementNoise, float estimateError);
};

class ExponentialFilter : public BaseFilter {
private:
    float _alpha;
    float _filteredValue;
    bool _initialized;

public:
    ExponentialFilter(float alpha);
    ~ExponentialFilter();

    float filter(float newValue) override;
    void reset() override;
    FilterType getType() const override;

    void setAlpha(float alpha);
};

struct FilterEntry {
    char *sensorName;
    char *valueKey;
    BaseFilter *filter;
    FilterType type;
    float lastFilteredValue;
    uint32_t lastUpdateTime;
};

class SensorFilterV2 {
private:
    FilterEntry **_filters;
    uint16_t _filterCount;
    uint16_t _filterCapacity;

    void cleanupFilterEntry(FilterEntry *entry);
    int findFilterIndex(const char *sensorName, const char *valueKey);

public:
    SensorFilterV2();
    ~SensorFilterV2();

    bool attachFilter(const char *sensorName, const char *valueKey,
                      FilterType type, FilterParams params);

    bool updateFilter(const char *sensorName, const char *valueKey,
                      FilterType type, FilterParams params);

    bool detachFilter(const char *sensorName, const char *valueKey);
    void detachAllFilters();

    float getFilteredValue(SensorModuleV2 *module, const char *sensorName, const char *valueKey);
    float getLastFilteredValue(const char *sensorName, const char *valueKey);

    void updateFilters(SensorModuleV2 *module);
    void resetFilter(const char *sensorName, const char *valueKey);
    void resetAllFilters();

    bool hasFilter(const char *sensorName, const char *valueKey);
    FilterType getFilterType(const char *sensorName, const char *valueKey);
};

#endif