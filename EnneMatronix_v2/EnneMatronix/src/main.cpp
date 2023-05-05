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

const int MOTOR_Y_STEP_PIN = 22;
const int MOTOR_Y_DIRECTION_PIN =23;
const int MOTOR_Y_ENABLE = 14;

const float minX = 0;
const float maxX = 450;

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

  stepperX.SetUp(MOTOR_X_STEP_PIN, MOTOR_X_DIRECTION_PIN, MOTOR_X_ENABLE);
  stepperX.SetUpLimits(true, minX, maxX);

  stepperY.SetUp(MOTOR_Y_STEP_PIN, MOTOR_Y_DIRECTION_PIN, MOTOR_Y_ENABLE);


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

  waitForEndStopRelease(button);
  delay(100);
}

long forward = 1;

void loop() 
{
    stepperX.moveToPosition(500);

    waitForEndStopFullPress(button);

    stepperX.moveToPosition(0);

    waitForEndStopFullPress(button);
}