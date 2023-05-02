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

const int MOTOR_X_STEP_PIN = 3;
const int MOTOR_X_DIRECTION_PIN = 2;
const int MOTOR_X_ENABLE = 26;

const int MOTOR_Y_STEP_PIN = 1;
const int MOTOR_Y_DIRECTION_PIN =0;
const int MOTOR_Y_ENABLE = 14;

void debugDisplay(const char* message)
{
    display.clearDisplay();
    display.setCursor(0,0);
    display.print(message);
    display.display();
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
  stepperY.SetUp(MOTOR_Y_STEP_PIN, MOTOR_Y_DIRECTION_PIN, MOTOR_Y_ENABLE);


  debugDisplay("Wait for end stop...");
  while (digitalRead(button) != LOW)
  {
    delay(100);
  }

  debugDisplay("Homing...");

  bool didHome = stepperY.Home(button);
  if(!didHome)
  {
    debugDisplay("Failed to home!");
  }
  else
  {
    debugDisplay("Home Sucessful!")
;  }
}

long forward = 1;

void loop() 
{
  bool pressed = digitalRead(button) == LOW;
  if(pressed)
  {
    stepperY.Stepper.moveRelativeInMillimeters(forward * 100);
    debugDisplay("Move!");
    forward *= -1;
  }
}