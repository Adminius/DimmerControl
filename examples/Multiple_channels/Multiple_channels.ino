#include "DimmerControl.h"

//create array of DimmerControls with unique IDs
DimmerControl channels[] = {
    DimmerControl(0),
    DimmerControl(1),
    DimmerControl(2),
    DimmerControl(3)
};

#define CHANNELS sizeof(channels) / sizeof(DimmerControl)

//define PINs
uint8_t pins[] = {8,9,10,11};

//channel is unique ID
void setChannelValue(byte id, byte value){
    analogWrite(pins[id], value);
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
    
    //Mandatory: set function that should be called to control output
    for(byte ch = 0;ch < CHANNELS; ch++){
        channels[ch].setValueIdFunction(&setChannelValue);
    }
    //turn first channel on
    channels[0].taskSoftOn();
}

void loop(){

    //task must be executed as often as possible
    for(byte ch = 0; ch < CHANNELS; ch++){
        channels[ch].task();
    }
    //if you want to inform user about current value it's better to check if it really needed:
    for(byte ch = 0; ch < CHANNELS; ch++){
        if(channels[ch].updateAvailable()){
            channels[ch].resetUpdateFlag();
            //toggle a state of the next channel: turn off it was on and vice versa
            channels[(ch + 1) % (CHANNELS)].taskToggleSoftOnOff();
        }
    }
}