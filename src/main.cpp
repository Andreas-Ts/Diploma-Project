
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <math.h>
#include <esp_now.h>
#include <WiFi.h>
#include "constants.h"
#include "customFunctions.h"

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme; // I2C
//Adafruit_BME680 bme(BME_CS); // hardware SPI
//Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);

void setup() {
  setReceiverMAC(MAC_LIBRARY[0]);
  int8_t potential_id = chooseIDBasedOfMCA();
  if (potential_id<0){
    Serial.println("Problem with reading the MCA address.");
    while(1) {}
  }
  else{
    id =  potential_id;
  }

    // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    while(1) {}
  }
  if (amITheReceiverESP32NOW()){
      esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  }
  else{
      setupSender();
  }
  
}

void loop() {
   loopSender();
}


void setupSender(){
     esp_now_register_send_cb(OnDataSent);
      memcpy(peerInfo.peer_addr, ESP32_MAC_OF_RECEIVER, 6);
      peerInfo.channel = 0;  
      peerInfo.encrypt = false;
  
      // Add peer        
     if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        while(1) {}
     }


}

void loopSender(){
  performBME680Reading(&bme,&myData);
  esp_now_send(ESP32_MAC_OF_RECEIVER, (uint8_t *) &myData, sizeof(myData)); 
}

// callback when data is sent from multiple senders
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
