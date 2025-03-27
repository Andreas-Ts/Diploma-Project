#include "custom_headers.h"


void setupSender(){
    
  Serial.println("Insideo setup sender");

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
  if (checkForInactivityOverThreshold(&timeLastMessageWasSend,maxWaitingTime)){
    errLeds();
  }
  if (waitingResponse == false){
      loopSensor(&message);
      esp_now_send(ESP32_MAC_OF_RECEIVER,(uint8_t *) &message, sizeof(sensorMessage)); 

  }
  
}

// callback when data is sent from multiple senders
void OnDataSentAsSender(const uint8_t *mac_addr, esp_now_send_status_t status) {
    //Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    if (status == ESP_NOW_SEND_SUCCESS){
      setTimerAndFlag(WAITING,&waitingResponse,&timeLastMessageWasSend);
    }
    else {
      setTimerAndFlag(FINISHED_SUCCESSFULLY,&waitingResponse,&timeLastMessageWasSend);
    }
  }
  
void OnDataRecvAsSender(const uint8_t *mac_addr,const uint8_t *incomingData, int len) {

  ResponseMessageFromReceiver *incomingDataStructified;
  memcpy(incomingDataStructified,incomingData,sizeof(incomingData));
  if (incomingDataStructified->writtenIntoQueue == true){
    setTimerAndFlag(FINISHED_SUCCESSFULLY,&waitingResponse,&timeLastMessageWasSend);    
  }
  else {
    setTimerAndFlag(FINISHED_UNSUCCESSFULLY,&waitingResponse,&timeLastMessageWasSend);
  }
}

