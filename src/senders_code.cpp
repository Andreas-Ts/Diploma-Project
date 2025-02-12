#include "customFunctions.h"
#include "constants.h"


void setupSender(){
    memcpy(peerInfo.peer_addr, ESP32_MAC_OF_RECEIVER, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    // Add peer        
   if (esp_now_add_peer(&peerInfo) != ESP_OK){
     // Serial.println("Failed to add peer connection to the broadcaster");
      is_Everything_Ok = false;
   }
   esp_now_register_send_cb(OnDataSentAsSender);
   esp_now_register_recv_cb(OnDataRecvAsSender);

  
}

void loopSender(){
is_Everything_Ok =  performBME680Reading(&bme,&myData,id);

esp_now_send(ESP32_MAC_OF_RECEIVER,(uint8_t *) &myData, sizeof(myData)); 
}

// callback when data is sent from multiple senders
void OnDataSentAsSender(const uint8_t *mac_addr, esp_now_send_status_t status) {
    //Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  }
  
void OnDataRecvAsSender(const uint8_t *mac_addr,const uint8_t *incomingData, int len) {

    sendDataToSerial(incomingData);

}

