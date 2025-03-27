#include "custom_headers.h"

void setupReceiver(){
    Serial.println("Inside setup Receiver");
    for (int i=0;i<ESP32_TOTAL_DEVICES_NUMBER;i++){
        
        Serial.println(i);
        if (!isTheReceiverESP32NOW( MAC_LIBRARY[i] )){
            
            memcpy(peerInfo.peer_addr, MAC_LIBRARY[i], 6);   
            if (esp_now_add_peer(&peerInfo) != ESP_OK){
               Serial.println("Failed to add peer connection to the broadcaster");
               is_Everything_Ok = false;
            }  
        }
    }
    if (!createMessageQueue(&receiverQueue,ESP32_TOTAL_DEVICES_NUMBER)){
        Serial.println("Problem while allocating memory for the queue.");
    }
   Serial.println("Before setting esp now setup Receiver");
    esp_now_register_send_cb(OnDataSentAsReceiver);
    esp_now_register_recv_cb(OnDataRecvAsReceiver);

}

void loopReceiver(){
    /*
    char readBuffer[5];
    
    if (Serial.available() > 0){

        Serial.readBytes(readBuffer,5);

        bool writtenSuccessfully;
        int id;

        memcpy(&id,readBuffer,sizeof(id));
        writtenSuccessfully = readBuffer[5];

        ResponseMessageFromReceiver response = createResponseFromReceiver(id,true,writtenSuccessfully);      
        setTimerAndFlag(FINISHED_SUCCESSFULLY,&waitingResponseSerial,&timeLastMessageWasSendSerial);

        esp_now_send(MAC_LIBRARY[response.id],(uint8_t *) &response, sizeof(response)); 
    }
    
    if (checkForInactivityOverThreshold(&timeLastMessageWasSendSerial,maxWaitingTimeSerial)){

        ResponseMessageFromReceiver response = createResponseFromReceiver(id,true,false);      
      
        esp_now_send(MAC_LIBRARY[response.id],(uint8_t *) &response, sizeof(response)); 

      }
    */    
    if (waitingResponse == false and isQueueFull(&receiverQueue) == false){

        loopSensor(&message);

        if (insertMessageIntoQueue(&receiverQueue,message)){
            setTimerAndFlag(FINISHED_SUCCESSFULLY,&waitingResponse,&timeLastMessageWasSend);    
          }
          else {
            setTimerAndFlag(FINISHED_UNSUCCESSFULLY,&waitingResponse,&timeLastMessageWasSend);
          }
           
    if ( isQueueEmpty(&receiverQueue) and Serial.availableForWrite() > MINIMUM_BYTE_TO_WRITE_AT_SERIAL){

        if (insertMessageIntoSerial(&receiverQueue)){
            setTimerAndFlag(FINISHED_SUCCESSFULLY,&waitingResponse,&timeLastMessageWasSend);    
          }
          else {
            setTimerAndFlag(FINISHED_UNSUCCESSFULLY,&waitingResponse,&timeLastMessageWasSend);
          }
        
    }
}


void OnDataSentAsReceiver(const uint8_t *mac_addr, esp_now_send_status_t status) {
    //We don't care what happen with the peer
    }
  
void OnDataRecvAsReceiver(const uint8_t *mac_addr,const uint8_t *incomingData, int len) {
    ResponseMessageFromReceiver response;
    sensorMessage *incomingDataStructified;
    memcpy(incomingDataStructified,incomingData,sizeof(incomingData));

    if (!insertMessageIntoQueue(&receiverQueue,*incomingDataStructified)){
        ResponseMessageFromReceiver response = createResponseFromReceiver(incomingDataStructified->id,false);      
        esp_now_send(MAC_LIBRARY[response.id],(uint8_t *) &response, sizeof(response)); 

    }
}
    

