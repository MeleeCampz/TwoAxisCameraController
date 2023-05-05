#include <Arduino.h>

#include<SpeedyStepper.h>


class Stepper
{
    private:
        bool isInit = false; 
        SpeedyStepper Stepper;

        bool hasLimit = false;
        float limitMin = 0;
        float limitMax = 0;

        float ClampPosition(float position)
        {
            if(!hasLimit) return position;
            return min(max(position, limitMin), limitMax);
        }

    public:
        void SetUp(int stepPin, int motorDirPin, int motorEnablePin)
        {
            //stepper 1
            Stepper.connectToPins(stepPin, motorDirPin);
            pinMode(motorEnablePin, OUTPUT);
            digitalWrite(motorEnablePin, LOW);
            Stepper.setSpeedInStepsPerSecond(1000*16);
            Stepper.setAccelerationInStepsPerSecondPerSecond(2000*16); 

            isInit = true;
        }

        void SetUpLimits(bool enable, float minPos, float maxPos)
        {
            hasLimit = enable;
            limitMin = minPos;
            limitMax = maxPos;
        } 

        bool Home(int switchPin)
        {
            if(!isInit) return;
            Stepper.setSpeedInMillimetersPerSecond(0);
            return Stepper.moveToHomeInSteps(-1, 200 * 10, 1000000, switchPin);
        }

        void moveToPosition(float position)
        {
            if(!isInit) return;
            position = ClampPosition(position);
            Stepper.moveToPositionInMillimeters(position);
        };
};