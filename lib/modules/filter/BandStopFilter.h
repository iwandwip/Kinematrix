#ifndef BAND_STOP_FILTER_H
#define BAND_STOP_FILTER_H

class BandStopFilter {
private:
    float a1, a2, b0, b1, b2; // Filter coefficients
    float x_n1, x_n2;         // Previous inputs
    float y_n1, y_n2;         // Previous outputs
    bool initialized;

public:
    BandStopFilter(float centerFreq, float bandwidth, float sampleRate);
    void reset();
    float filter(float input);
};

#endif
