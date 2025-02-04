
#include <Wire.h>
#include <SPI.h>

#include <math.h>
#include <esp_now.h>
#include <WiFi.h>
#include "constants.h"
#include "customFunctions.h"


 Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);



void setup() {
  Serial.println("hiiiiii");
  readMacAddress();
  setReceiverMAC(MAC_LIBRARY[2]);
  int8_t potential_id = chooseIDBasedOfMCA();
  if (potential_id<0){
    Serial.println("Problem with reading the MCA address.");
    while(1) {}
  }
  else{
    id =  potential_id;
    myData.id = id;
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
      esp_now_register_recv_cb(OnDataRecv);
  }
  else{
      setupSender();
  }
  
}

void loop() {
  Serial.println("hi");
  if (amITheReceiverESP32NOW()){
   loopReceiver();
  }
  else{
   loopSender();
   }
}

void setupSender(){
      memcpy(peerInfo.peer_addr, ESP32_MAC_OF_RECEIVER, 6);
      peerInfo.channel = 0;  
      peerInfo.encrypt = false;
      // Add peer        
     if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer connection to the broadcaster");
        while(1) {}
     }
    esp_now_register_send_cb(OnDataSent);
}

void loopSender(){
  performBME680Reading(&bme,&myData,id);

  esp_now_send(ESP32_MAC_OF_RECEIVER, (uint8_t *) &myData, sizeof(myData)); 
}
void loopReceiver(){
      performBME680Reading(&bme,&myData,id);
      Serial.print("I created this data.");
      sendDataToSerial((byte*) &myData,sizeof(myData));
}


// callback when data is sent from multiple senders
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t *mac_addr,const uint8_t *incomingData, int len) {
  char macStr[18];
  // Write the data into serial
  sendDataToSerial((byte*)incomingData,sizeof(incomingData));
}

