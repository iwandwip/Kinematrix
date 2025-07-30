#include "SensorFilterV2.h"
#include "../SensorModuleV2.h"

BaseFilter::BaseFilter() {
}

BaseFilter::~BaseFilter() {
}

void BaseFilter::reset() {
}

MovingAverageFilter::MovingAverageFilter(uint8_t windowSize)
        : _buffer(nullptr), _windowSize(windowSize > 0 ? windowSize : 1),
          _currentIndex(0), _count(0), _sum(0.0f) {
    _buffer = new float[_windowSize];
    for (uint8_t i = 0; i < _windowSize; i++) {
        _buffer[i] = 0.0f;
    }
}

MovingAverageFilter::~MovingAverageFilter() {
    if (_buffer) {
        delete[] _buffer;
    }
}

float MovingAverageFilter::filter(float newValue) {
    if (_count < _windowSize) {
        _sum += newValue;
        _count++;
    } else {
        _sum = _sum - _buffer[_currentIndex] + newValue;
    }

    _buffer[_currentIndex] = newValue;
    _currentIndex = (_currentIndex + 1) % _windowSize;

    return _sum / _count;
}

void MovingAverageFilter::reset() {
    if (_buffer) {
        for (uint8_t i = 0; i < _windowSize; i++) {
            _buffer[i] = 0.0f;
        }
    }

    _currentIndex = 0;
    _count = 0;
    _sum = 0.0f;
}

FilterType MovingAverageFilter::getType() const {
    return FILTER_MOVING_AVERAGE;
}

MedianFilter::MedianFilter(uint8_t windowSize)
        : _buffer(nullptr), _sortedBuffer(nullptr),
          _windowSize(windowSize > 0 ? windowSize : 1),
          _currentIndex(0), _count(0) {
    _buffer = new float[_windowSize];
    _sortedBuffer = new float[_windowSize];

    for (uint8_t i = 0; i < _windowSize; i++) {
        _buffer[i] = 0.0f;
        _sortedBuffer[i] = 0.0f;
    }
}

MedianFilter::~MedianFilter() {
    if (_buffer) {
        delete[] _buffer;
    }
    if (_sortedBuffer) {
        delete[] _sortedBuffer;
    }
}

float MedianFilter::filter(float newValue) {
    _buffer[_currentIndex] = newValue;
    _currentIndex = (_currentIndex + 1) % _windowSize;

    if (_count < _windowSize) {
        _count++;
    }

    sortBuffer();

    if (_count % 2 == 0) {
        return (_sortedBuffer[_count / 2 - 1] + _sortedBuffer[_count / 2]) / 2.0f;
    } else {
        return _sortedBuffer[_count / 2];
    }
}

void MedianFilter::reset() {
    if (_buffer && _sortedBuffer) {
        for (uint8_t i = 0; i < _windowSize; i++) {
            _buffer[i] = 0.0f;
            _sortedBuffer[i] = 0.0f;
        }
    }

    _currentIndex = 0;
    _count = 0;
}

FilterType MedianFilter::getType() const {
    return FILTER_MEDIAN;
}

void MedianFilter::sortBuffer() {
    for (uint8_t i = 0; i < _count; i++) {
        _sortedBuffer[i] = _buffer[i];
    }

    for (uint8_t i = 0; i < _count - 1; i++) {
        for (uint8_t j = 0; j < _count - i - 1; j++) {
            if (_sortedBuffer[j] > _sortedBuffer[j + 1]) {
                float temp = _sortedBuffer[j];
                _sortedBuffer[j] = _sortedBuffer[j + 1];
                _sortedBuffer[j + 1] = temp;
            }
        }
    }
}

KalmanFilter::KalmanFilter(float processNoise, float measurementNoise, float estimateError)
        : _processNoise(processNoise),
          _measurementNoise(measurementNoise),
          _estimateError(estimateError),
          _currentEstimate(0.0f),
          _lastEstimate(0.0f),
          _kalmanGain(0.0f),
          _initialized(false) {
}

KalmanFilter::~KalmanFilter() {
}

float KalmanFilter::filter(float newValue) {
    if (!_initialized) {
        _currentEstimate = newValue;
        _lastEstimate = newValue;
        _initialized = true;
        return newValue;
    }

    _estimateError = _estimateError + _processNoise;
    _kalmanGain = _estimateError / (_estimateError + _measurementNoise);
    _currentEstimate = _lastEstimate + _kalmanGain * (newValue - _lastEstimate);
    _estimateError = (1.0f - _kalmanGain) * _estimateError;
    _lastEstimate = _currentEstimate;

    return _currentEstimate;
}

void KalmanFilter::reset() {
    _currentEstimate = 0.0f;
    _lastEstimate = 0.0f;
    _kalmanGain = 0.0f;
    _initialized = false;
}

FilterType KalmanFilter::getType() const {
    return FILTER_KALMAN;
}

void KalmanFilter::setParameters(float processNoise, float measurementNoise, float estimateError) {
    _processNoise = processNoise;
    _measurementNoise = measurementNoise;
    _estimateError = estimateError;
}

ExponentialFilter::ExponentialFilter(float alpha)
        : _alpha(alpha > 0.0f && alpha <= 1.0f ? alpha : 0.5f),
          _filteredValue(0.0f),
          _initialized(false) {
}

ExponentialFilter::~ExponentialFilter() {
}

float ExponentialFilter::filter(float newValue) {
    if (!_initialized) {
        _filteredValue = newValue;
        _initialized = true;
        return newValue;
    }

    _filteredValue = _alpha * newValue + (1.0f - _alpha) * _filteredValue;
    return _filteredValue;
}

void ExponentialFilter::reset() {
    _filteredValue = 0.0f;
    _initialized = false;
}

FilterType ExponentialFilter::getType() const {
    return FILTER_EXPONENTIAL;
}

void ExponentialFilter::setAlpha(float alpha) {
    _alpha = alpha > 0.0f && alpha <= 1.0f ? alpha : 0.5f;
}

SensorFilterV2::SensorFilterV2()
        : _filters(nullptr), _filterCount(0), _filterCapacity(0) {
}

SensorFilterV2::~SensorFilterV2() {
    detachAllFilters();
}

bool SensorFilterV2::attachFilter(const char *sensorName, const char *valueKey,
                                  FilterType type, FilterParams params) {
    if (!sensorName || !valueKey || type == FILTER_NONE) {
        return false;
    }

    int existingIndex = findFilterIndex(sensorName, valueKey);
    if (existingIndex >= 0) {
        return updateFilter(sensorName, valueKey, type, params);
    }

    FilterEntry *newEntry = new FilterEntry();

    newEntry->sensorName = strdup(sensorName);
    newEntry->valueKey = strdup(valueKey);
    newEntry->type = type;
    newEntry->lastFilteredValue = 0.0f;
    newEntry->lastUpdateTime = 0;

    BaseFilter *filter;

    switch (type) {
        case FILTER_MOVING_AVERAGE:
            filter = new MovingAverageFilter(params.movingAverage.windowSize);
            break;

        case FILTER_MEDIAN:
            filter = new MedianFilter(params.median.windowSize);
            break;

        case FILTER_KALMAN:
            filter = new KalmanFilter(params.kalman.processNoise,
                                      params.kalman.measurementNoise,
                                      params.kalman.estimateError);
            break;

        case FILTER_EXPONENTIAL:
            filter = new ExponentialFilter(params.exponential.alpha);
            break;

        default:
            filter = nullptr;
            break;
    }

    if (!filter) {
        if (newEntry->sensorName) free(const_cast<char*>(newEntry->sensorName));
        if (newEntry->valueKey) free(const_cast<char*>(newEntry->valueKey));
        delete newEntry;
        return false;
    }

    newEntry->filter = filter;

    if (_filterCount >= _filterCapacity) {
        uint16_t newCapacity = _filterCapacity + 4;
        FilterEntry **newArray = (FilterEntry **) realloc(_filters, newCapacity * sizeof(FilterEntry *));

        if (!newArray) {
            cleanupFilterEntry(newEntry);
            return false;
        }

        _filters = newArray;
        _filterCapacity = newCapacity;
    }

    _filters[_filterCount++] = newEntry;
    return true;
}

bool SensorFilterV2::updateFilter(const char *sensorName, const char *valueKey,
                                  FilterType type, FilterParams params) {
    int index = findFilterIndex(sensorName, valueKey);
    if (index < 0 || type == FILTER_NONE) {
        return false;
    }

    FilterEntry *entry = _filters[index];

    if (entry->type == type) {
        switch (type) {
            case FILTER_MOVING_AVERAGE: {
                MovingAverageFilter *filter = static_cast<MovingAverageFilter *>(entry->filter);
                if (filter) {
                    filter->reset();
                }
                break;
            }

            case FILTER_MEDIAN: {
                MedianFilter *filter = static_cast<MedianFilter *>(entry->filter);
                if (filter) {
                    filter->reset();
                }
                break;
            }

            case FILTER_KALMAN: {
                KalmanFilter *filter = static_cast<KalmanFilter *>(entry->filter);
                if (filter) {
                    filter->setParameters(params.kalman.processNoise,
                                          params.kalman.measurementNoise,
                                          params.kalman.estimateError);
                    filter->reset();
                }
                break;
            }

            case FILTER_EXPONENTIAL: {
                ExponentialFilter *filter = static_cast<ExponentialFilter *>(entry->filter);
                if (filter) {
                    filter->setAlpha(params.exponential.alpha);
                    filter->reset();
                }
                break;
            }

            default:
                return false;
        }

        return true;
    } else {
        if (entry->filter) {
            delete entry->filter;
            entry->filter = nullptr;
        }

        BaseFilter *newFilter;

        switch (type) {
            case FILTER_MOVING_AVERAGE:
                newFilter = new MovingAverageFilter(params.movingAverage.windowSize);
                break;

            case FILTER_MEDIAN:
                newFilter = new MedianFilter(params.median.windowSize);
                break;

            case FILTER_KALMAN:
                newFilter = new KalmanFilter(params.kalman.processNoise,
                                             params.kalman.measurementNoise,
                                             params.kalman.estimateError);
                break;

            case FILTER_EXPONENTIAL:
                newFilter = new ExponentialFilter(params.exponential.alpha);
                break;

            default:
                newFilter = nullptr;
                break;
        }

        if (!newFilter) {
            return false;
        }

        entry->filter = newFilter;
        entry->type = type;
        entry->lastFilteredValue = 0.0f;

        return true;
    }
}

bool SensorFilterV2::detachFilter(const char *sensorName, const char *valueKey) {
    int index = findFilterIndex(sensorName, valueKey);
    if (index < 0) {
        return false;
    }

    cleanupFilterEntry(_filters[index]);

    for (uint16_t i = index; i < _filterCount - 1; i++) {
        _filters[i] = _filters[i + 1];
    }

    _filterCount--;
    return true;
}

void SensorFilterV2::detachAllFilters() {
    if (_filters) {
        for (uint16_t i = 0; i < _filterCount; i++) {
            cleanupFilterEntry(_filters[i]);
        }

        free(_filters);
        _filters = nullptr;
        _filterCount = 0;
        _filterCapacity = 0;
    }
}

float SensorFilterV2::getFilteredValue(SensorModuleV2 *module, const char *sensorName, const char *valueKey) {
    if (!module || !sensorName || !valueKey) {
        return 0.0f;
    }

    int index = findFilterIndex(sensorName, valueKey);
    if (index < 0) {
        BaseSensV2 *sensor = module->getSensorByName(sensorName);
        if (sensor) {
            return sensor->getFloatValue(valueKey);
        }
        return 0.0f;
    }

    BaseSensV2 *sensor = module->getSensorByName(sensorName);
    if (!sensor) {
        return _filters[index]->lastFilteredValue;
    }

    float rawValue = sensor->getFloatValue(valueKey);
    float filteredValue = _filters[index]->filter->filter(rawValue);

    _filters[index]->lastFilteredValue = filteredValue;
    _filters[index]->lastUpdateTime = millis();

    return filteredValue;
}

float SensorFilterV2::getLastFilteredValue(const char *sensorName, const char *valueKey) {
    int index = findFilterIndex(sensorName, valueKey);
    if (index < 0) {
        return 0.0f;
    }

    return _filters[index]->lastFilteredValue;
}

void SensorFilterV2::updateFilters(SensorModuleV2 *module) {
    if (!module || !_filters || _filterCount == 0) {
        return;
    }

    for (uint16_t i = 0; i < _filterCount; i++) {
        FilterEntry *entry = _filters[i];
        BaseSensV2 *sensor = module->getSensorByName(entry->sensorName);

        if (sensor) {
            float rawValue = sensor->getFloatValue(entry->valueKey);
            float filteredValue = entry->filter->filter(rawValue);

            entry->lastFilteredValue = filteredValue;
            entry->lastUpdateTime = millis();
        }
    }
}

void SensorFilterV2::resetFilter(const char *sensorName, const char *valueKey) {
    int index = findFilterIndex(sensorName, valueKey);
    if (index >= 0 && _filters[index]->filter) {
        _filters[index]->filter->reset();
        _filters[index]->lastFilteredValue = 0.0f;
    }
}

void SensorFilterV2::resetAllFilters() {
    if (!_filters || _filterCount == 0) {
        return;
    }

    for (uint16_t i = 0; i < _filterCount; i++) {
        if (_filters[i]->filter) {
            _filters[i]->filter->reset();
            _filters[i]->lastFilteredValue = 0.0f;
        }
    }
}

bool SensorFilterV2::hasFilter(const char *sensorName, const char *valueKey) {
    return findFilterIndex(sensorName, valueKey) >= 0;
}

FilterType SensorFilterV2::getFilterType(const char *sensorName, const char *valueKey) {
    int index = findFilterIndex(sensorName, valueKey);
    if (index < 0) {
        return FILTER_NONE;
    }

    return _filters[index]->type;
}

void SensorFilterV2::cleanupFilterEntry(FilterEntry *entry) {
    if (entry) {
        if (entry->sensorName) free(entry->sensorName);
        if (entry->valueKey) free(entry->valueKey);
        if (entry->filter) delete entry->filter;
        delete entry;
    }
}

int SensorFilterV2::findFilterIndex(const char *sensorName, const char *valueKey) {
    if (!_filters) {
        return -1;
    }

    for (uint16_t i = 0; i < _filterCount; i++) {
        if (strcmp(_filters[i]->sensorName, sensorName) == 0 &&
            strcmp(_filters[i]->valueKey, valueKey) == 0) {
            return i;
        }
    }

    return -1;
}