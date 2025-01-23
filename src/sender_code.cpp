/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp-now-many-to-one-esp32/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include "constants.h"
#include "customFunctions.h"



// Create a struct_message called myData
SensorMessage myData;

// Create peer interface



Adafruit_BME680 *bme;




 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // setBME680(bme);
} 
 
void loop() {
    
}

