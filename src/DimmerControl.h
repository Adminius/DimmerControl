/*
 *  DimmerControl.h - This library implements dimmer control functions
 *  Created by E.Burkoski, September 19, 2018
 *  Released into the public domain.
 */

#ifndef DimmerControl_h
#define DimmerControl_h

#include "Arduino.h"

#define DIM_IDLE    0
#define DIM_STOP    1
#define DIM_ON      2
#define DIM_OFF     3
#define DIM_SOFTON  4
#define DIM_SOFTOFF 5
#define DIM_UP      6
#define DIM_DOWN    7
#define DIM_SET     8

#define PS_OFF         0
#define PS_OFF_ONGOING 1
#define PS_ON          2
#define PS_ON_ONGOING  3

class DimmerControl
{
public:

    DimmerControl();
    DimmerControl(byte id);
    void task();

    //settings
    void setMinValue(byte valueMin);
    void setMaxValue(byte valueMax);
    void setDurationAbsolute(int durationAbsolute);
    void setDurationRelative(int durationRelative);
    void (*_setValueFunction)(byte);
    void (*_setValueIdFunction)(byte,byte);
    void setValueFunction(void (*setValueFunction)(byte));
    void setValueIdFunction(void (*setValueIdFunction)(byte,byte));
    void setPowerSupplyOnDelay(word onDelay);
    void setPowerSupplyOffDelay(unsigned long offDelay);

    //control
    void taskStop();
    void taskOn();
    void taskOff();
    void taskToggleOnOff();
    void taskSoftOn();
    void taskSoftOff();
    void taskToggleSoftOnOff();
    void taskDimUp();
    void taskDimDown();
    void taskToggleDimUpDown();
    void taskNewValue(byte valueNew);

    //information
    bool isBusy();
    bool getPowerSupplyState();
    bool getPowerSupplyOnRequest();
    byte getCurrentValue();
    byte getMinValue();
    byte getMaxValue();
    int getDurationAbsolute();
    int getDurationRelative();
    bool updateAvailable();
    void resetUpdateFlag();

private:
    byte _id;
    byte _valueMin;
    byte _valueMax;
    byte _valueNew;
    byte _valueCurrent;
    bool _dimDirection;
    byte _currentTask;
    byte _updateCounter;
    byte _updateInterval;
    bool _updateAvailable;
    bool _busy;
    bool _powerSupplyState;
    byte _powerSupplyTask;
    word _powerSupplyOnDelay;
    unsigned long _powerSupplyOffDelay;
    unsigned long _powerSupplyTaskMillis;
    int  _durationAbsolute;
    int  _durationRelative;
    word _delayAbsolute;
    word _delayRelative;
    unsigned long _lastTaskExecution;
    void calculateUpdateInterval();
    void setValue(byte value);
};

#endif