/*
 *  DimmerControl.cpp - This library implements dimmer control functions
 *  Created by E.Burkoski, September 19, 2018
 *  Released into the public domain.
 */

#include "DimmerControl.h"

DimmerControl::DimmerControl(byte id){
    _id = id;
    _valueMin = 0;
    _valueMax = 255;
    _valueNew = 0;
    _currentTask = DIM_IDLE;
    _updateCounter = 0;
    _updateAvailable = false;
    _dimDirection = true;
    _busy = false;
    _powerSupplyState = false;
    _powerSupplyTask = PS_OFF;
    _powerSupplyOnDelay = 0;
    _powerSupplyOffDelay = 0;
    _durationAbsolute = 500;
    _durationRelative = 5000;
    _lastTaskExecution = 0;
    calculateUpdateInterval();
}

DimmerControl::DimmerControl(){
    _id = 0;
    _valueMin = 0;
    _valueMax = 255;
    _valueNew = 0;
    _currentTask = DIM_IDLE;
    _updateCounter = 0;
    _updateAvailable = false;
    _dimDirection = true;
    _busy = false;
    _powerSupplyState = false;
    _powerSupplyTask = PS_OFF;
    _powerSupplyOnDelay = 0;
    _powerSupplyOffDelay = 0;
    _durationAbsolute = 500;
    _durationRelative = 5000;
    _lastTaskExecution = 0;
    calculateUpdateInterval();
}

void DimmerControl::task(){
    unsigned long _currentMillis = millis();
    switch(_currentTask) {
        //stop all activities
        case DIM_STOP:
            _busy = false;
            _updateAvailable = true;
            _updateCounter = 0;
            _currentTask = DIM_IDLE;
            if(_valueCurrent == 0 && _powerSupplyTask != PS_OFF_ONGOING && _powerSupplyTask != PS_OFF){
                _powerSupplyTaskMillis = _currentMillis;
                _powerSupplyTask = PS_OFF_ONGOING;
            }
            if(_valueCurrent == _valueMin)
                _dimDirection = true;
            if(_valueCurrent == _valueMax)
                _dimDirection = false;
            break;
        //turn on immediately
        case DIM_ON:
            if(_valueCurrent != _valueMax){
                if(_powerSupplyTask == PS_ON){
                    _valueCurrent = _valueMax;
                    setValue(_valueCurrent);
                    _currentTask = DIM_STOP;
                }else{
                    if(_powerSupplyTask != PS_ON_ONGOING){
                        _powerSupplyTaskMillis = _currentMillis;
                        _powerSupplyTask = PS_ON_ONGOING;
                    }
                }
            }
            break;
        //turn off immediately
        case DIM_OFF:
            if(_valueCurrent != _valueMin){
                _valueCurrent = _valueMin;
                setValue(_valueCurrent);
                _currentTask = DIM_STOP;
            }
            break;
        //smooth turn on
        case DIM_SOFTON:
            if(_valueCurrent < _valueMax){
                if(_powerSupplyTask == PS_ON){
                    if(!_busy){
                        _delayAbsolute = (word)(_durationAbsolute / (_valueMax - _valueCurrent));
                    }
                    if(_currentMillis - _lastTaskExecution >= _delayAbsolute){
                        _valueCurrent++;
                        _busy = true;
                        setValue(_valueCurrent);
                        _lastTaskExecution = millis();
                    }
                }else{
                    if(_powerSupplyTask != PS_ON_ONGOING){
                        _powerSupplyTaskMillis = _currentMillis;
                        _powerSupplyTask = PS_ON_ONGOING;
                    }
                }
            }else{
                _currentTask = DIM_STOP;
            }
            break;
        //smooth turn off
        case DIM_SOFTOFF:
            if(_valueCurrent > _valueMin){
                if(!_busy){
                    _delayAbsolute = (word)(_durationAbsolute / (_valueCurrent - _valueMin));
                }
                if(_currentMillis - _lastTaskExecution >= _delayAbsolute){
                    _valueCurrent--;
                    _busy = true;
                    setValue(_valueCurrent);
                    _lastTaskExecution = millis();
                }
            }else{
                _currentTask = DIM_STOP;
            }
            break;
        //increase value
        case DIM_UP:
            if(_valueCurrent < _valueMax){
                if(_powerSupplyTask == PS_ON){
                    if(!_busy){
                        _delayRelative = (word)(_durationRelative / (_valueMax - _valueCurrent));
                    }
                    if(_currentMillis - _lastTaskExecution >= _delayRelative){
                        _valueCurrent++;
                        _busy = true;
                        _updateCounter++;
                        if(_updateCounter >= _updateInterval){
                            _updateAvailable  = true;
                            _updateCounter = 0;
                        }
                        setValue(_valueCurrent);
                        _lastTaskExecution = millis();
                    }
                }else{
                    if(_powerSupplyTask != PS_ON_ONGOING){
                        _powerSupplyTaskMillis = _currentMillis;
                        _powerSupplyTask = PS_ON_ONGOING;
                    }
                }
            }else{
                _currentTask = DIM_STOP;
            }
            break;
        //decrease value
        case DIM_DOWN:
            if(_valueCurrent > _valueMin){
                if(!_busy){
                    _delayRelative = (word)(_durationRelative / (_valueCurrent - _valueMin));
                }
                if(_currentMillis - _lastTaskExecution >= _delayRelative){
                    _valueCurrent--;
                    _busy = true;
                    _updateCounter++;
                    if(_updateCounter >= _updateInterval){
                        _updateAvailable  = true;
                        _updateCounter = 0;
                    }
                    setValue(_valueCurrent);
                    _lastTaskExecution = millis();
                }
            }else{
                _currentTask = DIM_STOP;
            }
            break;
        //set value
        case DIM_SET:
            if(_valueCurrent < _valueNew){
                if(_powerSupplyTask == PS_ON){
                    if(!_busy){
                        _delayAbsolute = (word)(_durationAbsolute / (_valueNew - _valueCurrent));
                    }
                    if(_currentMillis - _lastTaskExecution >= _delayAbsolute){
                        _valueCurrent++;
                        _busy = true;
                        setValue(_valueCurrent);
                        _lastTaskExecution = millis();
                    }
                }else{
                    if(_powerSupplyTask != PS_ON_ONGOING){
                        _powerSupplyTaskMillis = _currentMillis;
                        _powerSupplyTask = PS_ON_ONGOING;
                    }
                }
            }else if(_valueCurrent > _valueNew){
                if(!_busy){
                    _delayAbsolute = (word)(_durationAbsolute / (_valueCurrent - _valueNew));
                }
                if(_currentMillis - _lastTaskExecution >= _delayAbsolute){
                    _valueCurrent--;
                    _busy = true;
                    setValue(_valueCurrent);
                    _lastTaskExecution = millis();
                }
            }else{
                _currentTask = DIM_STOP;
            }
            break;
        case DIM_IDLE:
        default:
            break;
    }

    switch(_powerSupplyTask) {
        case PS_ON_ONGOING:
            if(_currentMillis - _powerSupplyTaskMillis >= _powerSupplyOnDelay){
                _powerSupplyState = true;
                _powerSupplyTask = PS_ON;
            }
            break;

        case PS_OFF_ONGOING:
            if(_currentMillis - _powerSupplyTaskMillis >= _powerSupplyOffDelay){
                _powerSupplyState = false;
                _powerSupplyTask = PS_OFF;
            }
            break;

        case PS_ON:
        case PS_OFF:
        default:
            break;
    }
}

void DimmerControl::calculateUpdateInterval(){
    _updateInterval = (byte)((_valueMax - _valueMin)/ 10) + 1;
}

void DimmerControl::setValue(byte value){
    if(*_setValueFunction != NULL){
        _setValueFunction(value);
    }
    if(*_setValueIdFunction != NULL){
        _setValueIdFunction(_id, value);
    }
}

void DimmerControl::setMinValue(byte valueMin){
    _valueMin = valueMin;
    calculateUpdateInterval();
}

byte DimmerControl::getMinValue(){
    return _valueMin;
}

void DimmerControl::setMaxValue(byte valueMax){
    _valueMax = valueMax;
    calculateUpdateInterval();
}

byte DimmerControl::getMaxValue(){
    return _valueMax;
}

void DimmerControl::setDurationAbsolute(int durationAbsolute){
    _durationAbsolute = durationAbsolute;
}

int DimmerControl::getDurationAbsolute(){
    return _durationAbsolute;
}

void DimmerControl::setDurationRelative(int durationRelative){
    _durationRelative = durationRelative;
}

int DimmerControl::getDurationRelative(){
    return _durationRelative;
}

void DimmerControl::setValueFunction(void (*setValueFunction)(byte)){
    _setValueFunction = setValueFunction;
}

void DimmerControl::setValueIdFunction(void (*setValueIdFunction)(byte, byte)){
    _setValueIdFunction = setValueIdFunction;
}

void DimmerControl::taskStop(){
    _currentTask = DIM_STOP;
}

void DimmerControl::taskOn(){
    _currentTask = DIM_ON;
}

void DimmerControl::taskOff(){
    _currentTask = DIM_OFF;
}

void DimmerControl::taskToggleOnOff(){
    if(getCurrentValue() != _valueMin)
        _currentTask = DIM_OFF;
    else
        _currentTask = DIM_ON;
}

void DimmerControl::taskSoftOn(){
    _currentTask = DIM_SOFTON;
}

void DimmerControl::taskSoftOff(){
    _currentTask = DIM_SOFTOFF;
}

void DimmerControl::taskToggleSoftOnOff(){
    if(getCurrentValue() != _valueMin)
        _currentTask = DIM_SOFTOFF;
    else
        _currentTask = DIM_SOFTON;
}

void DimmerControl::taskDimUp(){
    _currentTask = DIM_UP;
}

void DimmerControl::taskDimDown(){
    _currentTask = DIM_DOWN;
}

void DimmerControl::taskToggleDimUpDown(){
    if(_dimDirection){
        _currentTask = DIM_UP;
        _dimDirection = false;
    }else{
        _currentTask = DIM_DOWN;
        _dimDirection = true;
    }
}

void DimmerControl::taskNewValue(byte valueNew){
    _valueNew = valueNew;
    _currentTask = DIM_SET;
}

bool DimmerControl::isBusy(){
    return _busy;
}

bool DimmerControl::getPowerSupplyState(){
    return _powerSupplyState;
}

void DimmerControl::setPowerSupplyOnDelay(word onDelay){
    _powerSupplyOnDelay = onDelay;
}

void DimmerControl::setPowerSupplyOffDelay(unsigned long offDelay){
    _powerSupplyOffDelay = offDelay;
}

byte DimmerControl::getCurrentValue(){
    return _valueCurrent;
}

bool DimmerControl::updateAvailable(){
    return _updateAvailable;
}

void DimmerControl::resetUpdateFlag(){
    _updateAvailable = false;
}