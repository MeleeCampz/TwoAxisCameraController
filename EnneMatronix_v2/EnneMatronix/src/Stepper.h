#include <Arduino.h>

#include<SpeedyStepper.h>


class Stepper
{
    public:
        void SetUp(int stepPin, int motorDirPin, int motorEnablePin);
        bool Home(int switchPin);

        float CurrentPosition;
        SpeedyStepper Stepper;
};