#include "custom_headers.h"

void setupReceiver(){
    //THE queue of the receiver
    for (int i=0;i<ESP32_TOTAL_DEVICES_NUMBER;i++){
        
        if (!isTheReceiverESP32NOW( MAC_LIBRARY[i] )){
            
            memcpy(peerInfo.peer_addr, MAC_LIBRARY[i], 6);   
            if (esp_now_add_peer(&peerInfo) != ESP_OK){
               is_Everything_Ok = false;
            }  
        }
    }

    if (!createMessageQueue(&receiverQueue,ESP32_TOTAL_DEVICES_NUMBER)){
        Serial.println("Problem while allocating memory for the queue.");
        errLeds();
    }
    esp_now_register_send_cb(OnDataSentAsReceiver);
    esp_now_register_recv_cb(OnDataRecvAsReceiver);
    
}

void loopReceiver(){

    //if it had taken too much time to take a response back from the python script,stop the receiver.
    //the other will stop as well eventually of their own
    if (checkForInactivityOverThreshold(timeLastMessageWasSend,maxWaitingTimeSerial)){
        Serial.println("Inactivity occured");
        errLeds();
      }
    
       
    if (Serial.available() > 0 and messageInSerialPending == true){
        //take space for one interger
        uint8_t* response =(uint8_t*) malloc(1);
        if (response == NULL){
            Serial.println("Memory not initialized");
            errLeds();
        }
        
        Serial.readBytes(response,1);

        bool writtenSuccessfully;
        
        if (idOfLastMessageInsertedIntoSerial == id){ //receiver send the last message
            if (*response){ //if readbuffer was true,so non zero value
                setFlag(NO_WAIT);
            }
            else{
                setFlag(WAIT_A_BIT);
            }

        }
        else{ //send message into the corresponsing peer device
            esp_now_send(MAC_LIBRARY[idOfLastMessageInsertedIntoSerial],response, sizeof(response)); 
        }
        free(response);
        messageInSerialPending = false;
    } 
    
   
  
    //check if the receiver waits to send a message and check for the queue for better results
    if (settingOfSensor == NO_WAIT and isQueueFull(&receiverQueue) == false 
    and (isTimeToSendMessage(settingOfSensor,timeLastMessageWasSend)))
    {
        loopSensor(&message);  
        //output = "Id of the new message is:" + String(message.id) +" and sensor type:" +String(message.sensor);
        //Serial.println(output);
        if (!insertMessageIntoQueue(&receiverQueue,message)){
            if (idOfLastMessageInsertedIntoSerial == id){ //receiver send the last message
                setFlag(WAIT_A_BIT);
           
    
            }
            else{ //send message into the corresponsing peer device that the insertion was happened
               
                uint8_t response = 0;
                esp_now_send(MAC_LIBRARY[idOfLastMessageInsertedIntoSerial],&response, 1); 
            }

        }
       // Serial.println("I inserted into the queue");
       // printQueueElements(&receiverQueue);


    }
    // Serial.println("I will print all the possible checks to write into Serial");
    // output = "messageInSerialPending is " + String(messageInSerialPending);
    // Serial.println(output);
    // output = "is QueueEmpty is " + String(isQueueEmpty(&receiverQueue));
    // Serial.println(output);
    // output = " Serial.availableForWrite is " +  String(Serial.availableForWrite());
    // Serial.println(output);
    //finally if queue is not empty,another message is not pending and we have enough space.send a message into serial
    if (isQueueEmpty(&receiverQueue) == false
    and Serial.availableForWrite() > MINIMUM_BYTE_TO_WRITE_AT_SERIAL
   ){
        insertMessageIntoSerial(&receiverQueue,&messageInSerialPending,&idOfLastMessageInsertedIntoSerial,&timeLastMessageWasSendInSerial);
        if (idOfLastMessageInsertedIntoSerial == id){ //receiver send the last message
            setFlag(NO_WAIT);
       

        }
        else{ //send message into the corresponsing peer device that the insertion was happened
           
            uint8_t response = 1;
            esp_now_send(MAC_LIBRARY[idOfLastMessageInsertedIntoSerial],&response, 1); 
        }
    }
}

void OnDataSentAsReceiver(const uint8_t *mac_addr, esp_now_send_status_t status) {
    //We don't care what happen with the peer
    }
  
void OnDataRecvAsReceiver(const uint8_t *mac_addr,const uint8_t *incomingData, int len) {
    //Serial.println("HI from receive receiver");
    uint8_t *response;
    sensorMessage *incomingDataStructified=(sensorMessage*) malloc(len);
    if (incomingDataStructified== NULL){
        perror("Failed to allocate memory");
        errLeds();
    }
    memcpy(incomingDataStructified,incomingData,len);
    //output = "Id of the new message is:" + String(incomingDataStructified->id) +" and sensor type:" +String(incomingDataStructified->sensor);
    //Serial.println(output);
    //If it was unsuccesful to insert the message into the queue, send false in order to make the peer device re send the message after ten seconds
    if (!insertMessageIntoQueue(&receiverQueue,*incomingDataStructified)){
        *response = 0;
        esp_now_send(MAC_LIBRARY[incomingDataStructified->id],response, 1); 
    }  

    free(incomingDataStructified);
}
    
