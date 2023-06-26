#include <Arduino.h>
#include "NetworkManager.h"

NetworkManager _manager;

void setup() 
{
  _manager.Begin();
}

void loop() 
{
  _manager.Update();
  delayMicroseconds(200);
}
