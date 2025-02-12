#include "customFunctions.h"
#include "constants.h"

void setReceiver(){
    for (int i=0;i<ESP32_TOTAL_DEVICES_NUMBER;i++){
        if (!isTheReceiverESP32NOW( MAC_LIBRARY[ESP32_TOTAL_DEVICES_NUMBER] )){

        }
    }
    esp_now_register_send_cb(OnDataSentAsReceiver);
    esp_now_register_recv_cb(OnDataRecvAsReceiver);

}

void loopReceiver(){
    is_Everything_Ok = performBME680Reading(&bme,&myData,id);
   
    //Serial.print("I created this data.");
    sendDataToSerial((uint8_t*) &myData ) ;   // Convert struct to bytes);
}


void OnDataSentAsReceiver(const uint8_t *mac_addr, esp_now_send_status_t status) {
    //Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  }
  
void OnDataRecvAsReceiver(const uint8_t *mac_addr,const uint8_t *incomingData, int len) {

    sendDataToSerial(incomingData);

}
