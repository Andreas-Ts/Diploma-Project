#include "customFunctions.h"
#include "constants.h"


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
  if (checkForInactivityOverThreshold(&timeLastMessageWasSend,maxWaitingTime)){
    esp_deep_sleep_start();
  }
  if (waitingResponse == false){
     is_Everything_Ok =  performBME680Reading(&bme,&sensorMessage,id);
  }
  if (is_Everything_Ok){
    esp_now_send(ESP32_MAC_OF_RECEIVER,(uint8_t *) &sensorMessage, sizeof(sensorMessage)); 
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
      //try again in 10 seconds
      delay(10 * 1000);
    }
  }
  
void OnDataRecvAsSender(const uint8_t *mac_addr,const uint8_t *incomingData, int len) {

  ResponseMessageFromReceiver *incomingDataStructified;
  memcpy(incomingDataStructified,incomingData,sizeof(incomingData));
  if (incomingDataStructified->writtenIntoQueue == false or incomingDataStructified->writtenSuccessfully == false){
    setTimerAndFlag(FINISHED_STANDBY,&waitingResponse,&timeLastMessageWasSend);

    //try again in 60 seconds
    delay(60 * 1000);
  }
  else {
    setTimerAndFlag(FINISHED_SUCCESSFULLY,&waitingResponse,&timeLastMessageWasSend);

  }
}

