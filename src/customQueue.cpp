
#include "custom_headers.h"

MessageQueue* createMessageQueue(const int sizeOfQueue) {

     
    MessageQueue *receiverQueue = (MessageQueue*)malloc(sizeof(MessageQueue));
    if (receiverQueue == NULL) {
      return NULL;  // error in allocation
    }
  
    receiverQueue->messages = (sensorMessage*)malloc(sizeOfQueue * sizeof(sensorMessage));
    if (receiverQueue->messages == NULL) {
      free(receiverQueue); //free previous memory allocated
      return NULL;
    }
    
    receiverQueue->maxSize = sizeOfQueue; //max size of queue
    receiverQueue->front = 0;
    receiverQueue->rear = 0;
    receiverQueue->currentSize = 0;
  
    return receiverQueue;
  }
void freeQueueMemory(MessageQueue* receiverQueue){

  free(receiverQueue->messages);
  free(receiverQueue);

}  
bool isQueueEmpty(MessageQueue* receiverQueue){
  if (receiverQueue->currentSize == 0){ 

    return true;
  }
  else{
    return false;
  }
}

bool isQueueFull(MessageQueue* receiverQueue){
  if (receiverQueue->currentSize == receiverQueue->maxSize){ 

    return true;
  }
  else{
    return false;
  }
}



bool insertMessageIntoQueue(MessageQueue* receiverQueue,sensorMessage message){
    
    if (isQueueFull){return false;}
    
    receiverQueue->rear = (receiverQueue->rear + 1) % receiverQueue->maxSize;
    receiverQueue->messages[receiverQueue->rear] = message;
    receiverQueue->currentSize++;
    return true;
  }
bool insertMessageIntoSerial(MessageQueue* receiverQueue){

    if (isQueueEmpty) {return false;}
    sensorMessage messageToSend;

    messageToSend = receiverQueue->messages[receiverQueue->front];
    
    sendDataToSerial(structToBytes(messageToSend));
    receiverQueue->front =(receiverQueue->front - 1) % receiverQueue->maxSize;
    receiverQueue->currentSize--;
    return true;
  }