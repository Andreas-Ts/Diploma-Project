#include "customFunctions.h"
#include "constants.h"

void setupReceiver(){
    for (int i=0;i<ESP32_TOTAL_DEVICES_NUMBER;i++){
        if (!isTheReceiverESP32NOW( MAC_LIBRARY[ESP32_TOTAL_DEVICES_NUMBER] )){
            memcpy(peerInfo.peer_addr, MAC_LIBRARY[ESP32_TOTAL_DEVICES_NUMBER], 6);      
            if (esp_now_add_peer(&peerInfo) != ESP_OK){
              // Serial.println("Failed to add peer connection to the broadcaster");
               is_Everything_Ok = false;
            }  
        }
    }
    MessageQueue* receiverQueue = createMessageQueue(ESP32_TOTAL_DEVICES_NUMBER);
    esp_now_register_send_cb(OnDataSentAsReceiver);
    esp_now_register_recv_cb(OnDataRecvAsReceiver);

}

void loopReceiver(){
    is_Everything_Ok = performBME680Reading(&bme,&myData,id);
    setTimerAndFlagWaiting();
    sendDataToSerial((uint8_t*) &myData ) ;   // Convert struct to bytes);
}


void OnDataSentAsReceiver(const uint8_t *mac_addr, esp_now_send_status_t status) {
    
    }
  
void OnDataRecvAsReceiver(const uint8_t *mac_addr,const uint8_t *incomingData, int len) {

    

    
}
void ReadFromSerial(){
    
}