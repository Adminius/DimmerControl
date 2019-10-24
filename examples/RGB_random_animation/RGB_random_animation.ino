#include "DimmerControl.h"

//create array of DimmerControls with unique IDs (ID represents a pin number)
DimmerControl channels[] = {
    DimmerControl(10), //red
    DimmerControl(11), //green
    DimmerControl(12)  //blue
};

#define CHANNELS sizeof(channels) / sizeof(DimmerControl)

unsigned long lastMillis[] = {0, 0, 0};
word colorDelay[] = {100, 200, 300};
word minDelay = 1000; //ms
word maxDelay = 6000; //ms
word turnOnOffTime = 10000; //ms

//unique ID = pin number
void setChannelValue(byte pin, byte value){
    analogWrite(pin, value);
}

void setup(){
    
    //Optional: set minimum possible value (default 0)
    //channels[x].setMinValue(0);
    //Optional: set maximum possible value (default 255)
    //channels[x].setMaxValue(255);
    //Optonal: how long should it if you turn on/off? (default 500ms)
    //channels[x].setDurationAbsolute(300); //milliseconds
    //Optonal: how long should it take if you dimming up/down (default 5000ms)
    //channels[x].setDurationRelative(4000); //milliseconds

    for(byte ch = 0;ch < CHANNELS; ch++){
        //Mandatory: set function that should be called to control output
        channels[ch].setValueIdFunction(&setChannelValue);
        channels[ch].setDurationAbsolute(turnOnOffTime); //milliseconds
    }
}

void loop(){
    unsigned long currentMillis = millis();

    for(byte ch = 0; ch < CHANNELS; ch++){
        //task must be executed as often as possible
        channels[ch].task();
        if(currentMillis - lastMillis[ch] >= colorDelay[ch]){
            channels[ch].taskToggleSoftOnOff();
            colorDelay[ch] = random(minDelay, maxDelay);
            lastMillis[ch] = currentMillis;
        }
    }
}