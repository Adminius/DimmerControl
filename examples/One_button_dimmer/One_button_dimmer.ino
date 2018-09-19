#include "DimmerControl.h"

DimmerControl led;

//pin settings
#define PIN_LED LED_BUILTIN
#define PIN_BTN 2

//global variables
bool boolButtonPressed = false;
bool duringLongClick = false;
bool dimDirection = true; //true: dim up, false: dim down
byte debounceDuration = 80; //ms
word longClickDuration = 400; //ms
unsigned long lastMillis = 0;

void taskButton(){
    unsigned long currentMillis = millis();
    bool buttonState = !digitalRead(PIN_BTN); //active low

    //button pressed
    if(!boolButtonPressed && buttonState){ 
        boolButtonPressed = true;
        lastMillis = currentMillis;
    }

    //button released
    if(boolButtonPressed && !buttonState){
        boolButtonPressed = false;
        //if (debounceDuration <= CLICKDURATION < longClickDuration) then it was a short click
        if((currentMillis - lastMillis >= debounceDuration) && 
           (currentMillis - lastMillis < longClickDuration)){
            //short click detected
            led.taskToggleSoftOnOff();
        }
    }

    //button is still pressed and it is a first long press detection
    if(boolButtonPressed && !duringLongClick && (currentMillis - lastMillis >= longClickDuration)){
        if(dimDirection){
            led.taskDimUp();
            dimDirection = false;
        }else{
            led.taskDimDown();
            dimDirection = true;
        }
        duringLongClick = true;
    }

    //button released after a long press
    if(!boolButtonPressed && duringLongClick && (currentMillis - lastMillis >= longClickDuration)){
        led.taskStop();
        duringLongClick = false;
    }
}

void setValue(byte value){
    analogWrite(PIN_LED, value);
}

void setup(){

    //Optional: set minimum possible value (default 0)
    //led.setMinValue(0);
    //Optional: set maximum possible value (default 255)
    //led.setMaxValue(255);
    //Optonal: how long should it if you turn on/off? (default 500ms)
    //led.setDurationAbsolute(300); //milliseconds
    //Optonal: how long should it take if you dimming up/down (default 5000ms)
    //led.setDurationRelative(4000); //milliseconds
    
    //Mandatory: set function that should be called to control output (
    led.setValueFunction(&setValue);
    pinMode(PIN_BTN, INPUT);
}

void loop(){
    //task must be executed as often as posible
    led.task();
    taskButton();
    if(led.updateAvailable()){
        if(led.getCurrentValue() == led.getMinValue()) dimDirection = true;
        if(led.getCurrentValue() == led.getMaxValue()) dimDirection = false;
        //reset the update flag
        led.resetUpdateFlag();
    }
}