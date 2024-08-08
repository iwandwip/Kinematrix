/*
 *  KeedAutoLight.h
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef KEED_AUTO_LIGHT_H
#define KEED_AUTO_LIGHT_H

#include "AutoLightConfig/KeedIndex.h"

class KeedAutoLight {
private:
    configuration_t cfg;
    IOExpander **ioBase;
    KeedBase *keedBase;
    uint8_t ioLen;
    bool isReadyRun;

    void addIoExpander(IOExpander *ioExpander);
    bool beginExpander();
    KeedBase *getChannel();
    int getIndex();
    bool isUsingExpander() const;

public:
    KeedAutoLight();
    ~KeedAutoLight();
    cfg_error_t setChannel(configuration_t _cfg);
    cfg_error_t init();

    void runAutoLight();
    void showInfo();
    bool isReady();

    void setInterruptConfig(interrupt_t _cfg);
    interrupt_t getInterruptConfig();
    void changeModes();
    void readModes();
    void setBaseDelay(uint32_t _time);

    IOExpander *getIoExpander(uint8_t index);
    IOExpander &getIoExpanderRef(uint8_t index);
    IOExpander **getIoExpanderPtr();

    KeedBase *getChannelClassPtr();
    KeedBase &getChannelClass();
};

#endif // KEED_AUTO_LIGHT_H