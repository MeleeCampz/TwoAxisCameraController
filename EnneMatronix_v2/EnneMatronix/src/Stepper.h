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
        bool homeDirection;
        bool flipDirection;

        float ClampPosition(float position)
        {
            if(!hasLimit) return position;
            return min(max(position, limitMin), limitMax);
        }

    public:
        void SetUp(int stepPin, int motorDirPin, int motorEnablePin, float stepsPerMillimeter, float velocity, float accel, bool homeDirection, bool flipDirection)
        {
            //stepper 1
            Stepper.connectToPins(stepPin, motorDirPin);
            pinMode(motorEnablePin, OUTPUT);
            digitalWrite(motorEnablePin, LOW);
            Stepper.setStepsPerMillimeter(stepsPerMillimeter);

            this->maxVelocity = velocity;
            this->acceleration = accel;
            this->homeDirection = homeDirection;
            this->flipDirection = flipDirection;


            Stepper.setSpeedInStepsPerSecond(maxVelocity);
            Stepper.setAccelerationInStepsPerSecondPerSecond(acceleration); 

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
            if(!isInit) return false;
            bool success = Stepper.moveToHomeInSteps(homeDirection ? -1 : 1, 200 * 10, 1000000, switchPin);
            
            return success;
        }

        void moveToPosition(float position)
        {
            if(!isInit) return;
            position = ClampPosition(position);
            Stepper.moveToPositionInMillimeters(flipDirection ? -position : position);
        }

        void disableMotor()
        {
            Stepper.disableStepper();
        }
};