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
    esp_now_register_send_cb(OnDataSentAsReceiver);
    esp_now_register_recv_cb(OnDataRecvAsReceiver);

}

void loopReceiver(){
    byte readBuffer[5];
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
    if (waitingResponse == false and isQueueFull(receiverQueue) == false){
        
        is_Everything_Ok = performBME680Reading(&bme,&sensorMessage,id);
        if (is_Everything_Ok){
            setTimerAndFlag(WAITING,&waitingResponse,&timeLastMessageWasSend);
            insertMessageIntoQueue(receiverQueue,sensorMessage);
        }    
        
    }
    if ( isQueueEmpty(receiverQueue) and Serial.availableForWrite()> MINIMUM_BYTE_TO_WRITE_AT_SERIAL){
        insertMessageIntoSerial(receiverQueue);
        setTimerAndFlag(WAITING,&waitingResponseSerial,&timeLastMessageWasSendSerial);
    }
}


void OnDataSentAsReceiver(const uint8_t *mac_addr, esp_now_send_status_t status) {
    //We don't care what happen with the peer
    }
  
void OnDataRecvAsReceiver(const uint8_t *mac_addr,const uint8_t *incomingData, int len) {

    SensorMessage *incomingDataStructified;
    memcpy(incomingDataStructified,incomingData,sizeof(incomingData));

    if (isQueueFull){
       
        ResponseMessageFromReceiver response = createResponseFromReceiver(incomingDataStructified->id,false,false);      
        esp_now_send(MAC_LIBRARY[response.id],(uint8_t *) &response, sizeof(response)); 
    }
    else{
        insertMessageIntoQueue(receiverQueue,*incomingDataStructified);

    }
    
}
