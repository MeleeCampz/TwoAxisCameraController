#include <SPI.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include<Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include<SpeedyStepper.h>

const int button = 18;

//const int LCD_POWER = 18;
//const int LCD_SCL = 19;
//const int LCD_SDA =20;

const int MOTOR1_STEP_PIN = 3;
const int MOTOR1_DIRECTION_PIN = 2;
const int MOTOR1_ENABLE = 26;

const int MOTOR2_STEP_PIN = 1;
const int MOTOR2_DIRECTION_PIN =0;
const int MOTOR2_ENABLE = 14;

SpeedyStepper stepper1;
SpeedyStepper stepper2;

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
  
  //stepper 1
  stepper1.connectToPins(MOTOR1_STEP_PIN, MOTOR1_DIRECTION_PIN);
  pinMode(MOTOR1_ENABLE, OUTPUT);
  digitalWrite(MOTOR1_ENABLE, LOW);
  stepper1.setSpeedInStepsPerSecond(1000*16);
  stepper1.setAccelerationInStepsPerSecondPerSecond(2000*16);  

  //stepper 2
  stepper2.connectToPins(MOTOR2_STEP_PIN, MOTOR2_DIRECTION_PIN);
  pinMode(MOTOR2_ENABLE, OUTPUT);
  digitalWrite(MOTOR2_ENABLE, LOW);
  stepper2.setSpeedInStepsPerSecond(1000*16);
  stepper2.setAccelerationInStepsPerSecondPerSecond(2000*16);  
}

void loop() 
{
  bool pressed = digitalRead(button) == LOW;
  if(pressed)
  {
    stepper1.moveRelativeInSteps(200*16);
    stepper2.moveRelativeInSteps(200*16);
    debugDisplay("Pressed!");
  }
}

void debugDisplay(char* message)
{
    display.clearDisplay();
    display.setCursor(0,0);
    display.print(message);
    display.display();
}
