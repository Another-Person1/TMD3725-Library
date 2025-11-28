#include <Wire.h>
#include "TMD3725.h"

TMD3725 tmd;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("TMD3725 BusIO Test");

  // Wire.begin(SDA, SCL); // Optional: Specify pins for ESP32
  
  if (!tmd.begin()) {
    Serial.println("Failed to find TMD3725 sensor!");
    while (1) delay(10);
  }
  Serial.println("Sensor Initialized.");

  tmd.enable(); 
  tmd.setALSGain(TMD3725_GAIN_16X);
  tmd.setIntegrationTime(0x23); // ~100ms
}

void loop() {
  TMD3725_Data data;
  
  if (tmd.getData(data)) {
    Serial.print("Prox: "); Serial.print(data.proximity);
    Serial.print("\tClear: "); Serial.print(data.clear);
    Serial.print("\tRed: "); Serial.print(data.red);
    Serial.print("\tGreen: "); Serial.print(data.green);
    Serial.print("\tBlue: "); Serial.println(data.blue);
  }
  
  delay(100);
}
