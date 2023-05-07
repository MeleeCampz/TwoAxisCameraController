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

        float maxVelocity;
        float acceleration;

        float ClampPosition(float position)
        {
            if(!hasLimit) return position;
            return min(max(position, limitMin), limitMax);
        }

    public:
        void SetUp(int stepPin, int motorDirPin, int motorEnablePin, float stepsPerMillimeter, float velocity, float accel)
        {
            //stepper 1
            Stepper.connectToPins(stepPin, motorDirPin);
            pinMode(motorEnablePin, OUTPUT);
            digitalWrite(motorEnablePin, LOW);
            Stepper.setStepsPerMillimeter(stepsPerMillimeter);

            maxVelocity = velocity;
            acceleration = accel;


            Stepper.setSpeedInStepsPerSecond(maxVelocity);
            Stepper.setAccelerationInStepsPerSecondPerSecond(acceleration); 

            isInit = true;
        }

        void SetUp(int stepPin, int motorDirPin, int motorEnablePin, float stepsPerMillimeter)
        {
            SetUp(stepPin, motorDirPin, motorEnablePin, stepsPerMillimeter, 2000 * 16,  2000 * 16);
        }

        void SetUpLimits(bool enable, float minPos, float maxPos)
        {
            hasLimit = enable;
            limitMin = minPos;
            limitMax = maxPos;
        } 

        bool Home(int switchPin)
        {
            if(!isInit) return false;
            bool success = Stepper.moveToHomeInSteps(-1, 200 * 10, 1000000, switchPin);
            
            return success;
        }

        void moveToPosition(float position)
        {
            if(!isInit) return;
            position = ClampPosition(position);
            Stepper.moveToPositionInMillimeters(position);
        };
};