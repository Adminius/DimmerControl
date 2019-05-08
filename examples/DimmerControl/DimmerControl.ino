#include "DimmerControl.h"

DimmerControl led1;

void setValue(byte value){
    analogWrite(LED_BUILTIN, value);
}

void setup(){
    Serial.begin(115200);
    //you have 3 seconds to open serial monitor
    delay(3000);
    Serial.println("Example sketch for DimmerControl");

    //Optional: set minimum possible value (default 0)
    //led1.setMinValue(0);
    //Optional: set maximum possible value (default 255)
    //led1.setMaxValue(255);
    //Optonal: how long should it if you turn on/off? (default 500ms)
    //led1.setDurationAbsolute(300); //milliseconds
    //Optonal: how long should it take if you dimming up/down (default 5000ms)
    //led1.setDurationRelative(4000); //milliseconds
    
    //Mandatory: set function that should be called to control output (
    led1.setValueFunction(&setValue);

    Serial.println("");
    Serial.println("");
    Serial.println("***  SETTINGS  ***");
    Serial.print("Minimum value: ");
    Serial.println(led1.getMinValue());
    Serial.print("Maximum value: ");
    Serial.println(led1.getMaxValue());
    Serial.print("Duration absolute [ms]: ");
    Serial.println(led1.getDurationAbsolute());
    Serial.print("Duration relative [ms]: ");
    Serial.println(led1.getDurationRelative());
    Serial.println("");
    Serial.println("");
    Serial.println("***  CONTROL  ***");
    Serial.println("Possible values:");
    Serial.println("1 = turn on");
    Serial.println("2 = turn off");
    Serial.println("3 = toggle on/off");
    Serial.println("4 = soft turn on");
    Serial.println("5 = soft turn off");
    Serial.println("6 = toggle soft on/off");
    Serial.println("7 = dim to newValue (random)");
    Serial.println("8 = dim up");
    Serial.println("9 = dim down");
    Serial.println("0 = dim stop");
}

void loop(){
    // check user input in serial monitor
    if(Serial.available() > 0){
        //Serial input returns char back, but we need a real number, so substitute 48 to match ASCII number
        byte nextTask = Serial.read() - 48;
        switch(nextTask){
            case 1:
                //turn completely on
                led1.taskOn();
                break;
            case 2:
                //turn completely off
                led1.taskOff();
                break;
            case 3:
                //toggle on/off
                led1.taskToggleOnOff();
                break;
            case 4:
                //turn completely on
                led1.taskSoftOn();
                break;
            case 5:
                //turn completely off
                led1.taskSoftOff();
                break;
            case 6:
                //toggle on/off
                led1.taskToggleSoftOnOff();
                break;
            case 7:
                //set new value... generate random value for example
                led1.taskNewValue(random(led1.getMinValue(), led1.getMaxValue()));
                break;
            case 8:
                //dimming up up to max value or until stop
                led1.taskDimUp();
                break;
            case 9:
                //dimming down up to min value or until stop
                led1.taskDimDown();
                break;
            case 0:
                //stop
                led1.taskStop();
                break;
            default:
                break; 
        }
    }
    //task must be executed as often as possible
    led1.task();
    //if you want to inform user about current value it's better to check if it really needed:
    if(led1.updateAvailable()){
        Serial.print("Current value:" );
        //get the current value and print it
        Serial.println(led1.getCurrentValue());
        //reset the update flag
        led1.resetUpdateFlag();
    }
}