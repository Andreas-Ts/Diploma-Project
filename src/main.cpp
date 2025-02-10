
#include <Wire.h>
#include <SPI.h>

#include <math.h>
#include <esp_now.h>
#include <WiFi.h>
#include <constants.h>
#include <customFunctions.h>

void setupSender();
void loopSender();
void setupReceiver();
void loopReceiver();

Adafruit_BME680 bme; // I2C
bool is_Everything_Ok = true;


void setup() {
   Serial.begin(115200);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  delay(50);
  Serial.println("hiiiiii");
 
  readMacAddress();
  setReceiverMAC(MAC_LIBRARY[1]);
  
  int potential_id = chooseIDBasedOfMCA();
  if (potential_id<0){
    Serial.println("Problem with reading the MCA address.");
    is_Everything_Ok= false;
  }
  else{
    Serial.print("The id of device choosen:");
    Serial.println(potential_id);

    id =  potential_id;
    myData.id = id;
  }
  //Serial.println("After MAC");
 // Serial.println(is_Everything_Ok);
 // delay(10);
  // Setup Sensor
  is_Everything_Ok = setBME680(&bme);
  //Serial.println("After SETUP SENSOR");
 // Serial.println(is_Everything_Ok);
 // delay(10);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    //Serial.println("Error initializing ESP-NOW");
    is_Everything_Ok= false;
  }
  //Serial.println("After SETUP ESP NOW ");
  Serial.println(is_Everything_Ok);
  delay(10);
  if (amITheReceiverESP32NOW()){
      esp_now_register_recv_cb(OnDataRecv);
  }
  else{
      setupSender();
  }
  
}

void loop() {
  if (is_Everything_Ok == false){
     Serial.println("An error occurred into the program. Shutdown the device");
  }
  do {} while (is_Everything_Ok == false);
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
       // Serial.println("Failed to add peer connection to the broadcaster");
        is_Everything_Ok = false;
     }
    esp_now_register_send_cb(OnDataSent);
}

void loopSender(){
  is_Everything_Ok =  performBME680Reading(&bme,&myData,id);

  esp_now_send(ESP32_MAC_OF_RECEIVER,(uint8_t *) &myData, sizeof(myData)); 
}
void loopReceiver(){
      is_Everything_Ok = performBME680Reading(&bme,&myData,id);
     
      //Serial.print("I created this data.");
      sendDataToSerial((uint8_t*) &myData ) ;   // Convert struct to bytes);
      delay(1000);
}


// callback when data is sent from multiple senders
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t *mac_addr,const uint8_t *incomingData, int len) {
  
  sendDataToSerial(incomingData);
  
  delay(1000);
}

