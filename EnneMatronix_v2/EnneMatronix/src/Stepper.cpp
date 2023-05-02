#include "Stepper.h"

    
void Stepper::SetUp(int stepPin, int motorDirPin, int motorEnablePin)
{
    //stepper 1
    Stepper.connectToPins(stepPin, motorDirPin);
    pinMode(motorEnablePin, OUTPUT);
    digitalWrite(motorEnablePin, LOW);
    Stepper.setSpeedInStepsPerSecond(1000*16);
    Stepper.setAccelerationInStepsPerSecondPerSecond(2000*16); 
}

bool Stepper::Home(int switchPin)
{
    Stepper.setSpeedInMillimetersPerSecond(0);
    return Stepper.moveToHomeInSteps(-1, 200 * 10, 1000000, switchPin);
}