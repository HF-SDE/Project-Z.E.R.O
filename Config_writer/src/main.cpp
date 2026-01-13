#include <Arduino.h>
#include "../lib/ConfigWriter.h"
#include "../lib/StorageManager.h"

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial)
  {
    ; // Wait for Serial to be ready
  }
  Serial.println("Hello world...");

  writeDefaultConfig();
  displayStoredConfig();
}

void loop()
{
  // put your main code here, to run repeatedly:
}
