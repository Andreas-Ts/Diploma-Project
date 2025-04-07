#include "custom_headers.h"


void setupSender(){
    

   // Add peer  
   memcpy(peerInfo.peer_addr, ESP32_MAC_OF_RECEIVER, 6);      
   if (esp_now_add_peer(&peerInfo) != ESP_OK){
     // Serial.println("Failed to add peer connection to the broadcaster");
      is_Everything_Ok = false;
   }
   esp_now_register_send_cb(OnDataSentAsSender);
   esp_now_register_recv_cb(OnDataRecvAsSender);

   
}

void loopSender(){
  if (checkForInactivityOverThreshold(timeLastMessageWasSend,maxWaitingTime)){
    Serial.println("Some error occured");
      errLeds();
  }
  if (settingOfSensor == NO_WAIT){
      if (isTimeToSendMessage(settingOfSensor,timeLastMessageWasSend)){
        loopSensor(&message);
        esp_now_send(ESP32_MAC_OF_RECEIVER,(uint8_t *) &message, sizeof(sensorMessage)); 
      }
     
  }
  
}

// callback when data is sent from multiple senders
void OnDataSentAsSender(const uint8_t *mac_addr, esp_now_send_status_t status) {
    //Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    if (status == ESP_NOW_SEND_SUCCESS){
      setTimerAndFlag(WAIT_FOR_RESPONSE);
    }
    else {
      setTimerAndFlag(WAIT_10_SECONDS);
    }
  }
//We will check for true if the message was written into the python or not,in that case we will wait 10 seconds.
void OnDataRecvAsSender(const uint8_t *mac_addr,const uint8_t *incomingData, int len) {

 
  if (*incomingData == true){
    setTimerAndFlag(NO_WAIT);    
  }
  else {
    setTimerAndFlag(WAIT_10_SECONDS);
  }
}

