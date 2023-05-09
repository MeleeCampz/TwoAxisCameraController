#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include<Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include "Stepper.h"

Stepper stepperX;
Stepper stepperY;

const int button = 18;

const int MOTOR_X_STEP_PIN = 15;
const int MOTOR_X_DIRECTION_PIN = 21;
const int MOTOR_X_ENABLE =14;
const float MOTOR_X_STEPS_PER_MILLIMETER = 80;

const int MOTOR_Y_STEP_PIN = 22;
const int MOTOR_Y_DIRECTION_PIN =23;
const int MOTOR_Y_ENABLE = 14;
//simply use degrees in stead of mm for rotation
const int MOTOR_Y_STEPS_PER_DEGREE = 45;

const float SLIDER_LENGTH = 380;
const float MARGIN = 15;
const float MIN_X = MARGIN;
const float MAX_X = SLIDER_LENGTH - MARGIN;

void debugDisplay(const char* message)
{
    display.clearDisplay();
    display.setCursor(0,0);
    display.print(message);
    display.display();
}

void waitForEndStopPress(int buttonID)
{
  while (digitalRead(buttonID) != LOW)
  {
    delay(100);
  }
}

void waitForEndStopRelease(int buttonID)
{
  while (digitalRead(buttonID) == LOW)
  {
    delay(100);
  }
}

void waitForEndStopFullPress(int buttonID)
{
  waitForEndStopPress(buttonID);
  waitForEndStopRelease(buttonID);
}

const int MAX_BUFFER_SIZE = 8;
char receive_buffer[MAX_BUFFER_SIZE];
bool newData = false;

void readSerialInput()
{
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
    
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (rc != endMarker) {
            receive_buffer[ndx] = rc;
            ndx++;
            if (ndx >= MAX_BUFFER_SIZE) {
                ndx = MAX_BUFFER_SIZE - 1;
            }
        }
        else {
            receive_buffer[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
    }
}

void setup() 
{
  //Button
  Serial.begin(115200);
  pinMode(button, INPUT_PULLUP);

  //OLED
  Serial.println("Starting OLED Display...");
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c))
  {
    Serial.println("Failed to start display..");
  }
  else
  {
    Serial.println("Success!");
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    debugDisplay("Display init success!");
  }

  stepperX.SetUp(MOTOR_X_STEP_PIN, MOTOR_X_DIRECTION_PIN, MOTOR_X_ENABLE, MOTOR_X_STEPS_PER_MILLIMETER);
  stepperX.SetUpLimits(true, MIN_X, MAX_X);

  stepperY.SetUp(MOTOR_Y_STEP_PIN, MOTOR_Y_DIRECTION_PIN, MOTOR_Y_ENABLE, MOTOR_Y_STEPS_PER_DEGREE);
  stepperY.SetUpLimits(true, -90, 90);

  debugDisplay("Wait for end stop...");
  waitForEndStopFullPress(button);

  delay(100);

  debugDisplay("Homing...");

  bool didHome = stepperX.Home(button);
  if(!didHome)
  {
    debugDisplay("Failed to home!");
  }
  else
  {
    debugDisplay("Home Sucessful!");
  }

  //Move to min pos to not touch stepper
  stepperX.moveToPosition(0);
}

void loop() 
{
    readSerialInput();

    if(newData)
    {
      //Number in percent 0 - 100
      int value = atoi(receive_buffer);

      float targetPosition =  MIN_X + (float)value / 100.f * SLIDER_LENGTH - 2 * MARGIN;

      stepperX.moveToPosition(targetPosition);
      newData = false;
    }
}