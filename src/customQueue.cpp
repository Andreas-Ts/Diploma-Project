
#include "constants.h"
#include "customFunctions.h"

MessageQueue* createMessageQueue(const int sizeOfQueue) {
    MessageQueue *queue = (MessageQueue*)malloc(sizeof(MessageQueue));
    if (queue == NULL) {
      return NULL;  // error in allocation
    }
  
    queue->messages = (SensorMessage*)malloc(sizeOfQueue * sizeof(SensorMessage));
    if (queue->messages == NULL) {
      free(queue); //free previous memory allocated
      return NULL;
    }
    
    queue->maxSize = sizeOfQueue; //max size of queue
    queue->front = 0;
    queue->rear = 0;
    queue->currentSize = 0;
  
    return queue;
  }
void freeQueueMemory(MessageQueue* queue){

  free(queue->messages);
  free(queue);

}  

bool insertMessageIntoQueue(MessageQueue* queue,SensorMessage message){
    //Check if queue is at max size
    if (queue->currentSize == queue->maxSize ){ 

      return false;
    }
    queue->rear = (queue->rear + 1) % queue->maxSize;
    queue->messages[queue->rear] = message;
    queue->currentSize++;
    return true;
  }
bool insertMessageIntoSerial(MessageQueue* queue){
    SensorMessage messageToSend;
    if (queue->currentSize == 0){ 

      return false;
    }
    messageToSend = queue->messages[queue->front];
    
    sendDataToSerial(structToBytes(messageToSend));
    queue->front =(queue->front - 1) % queue->maxSize;
    queue->currentSize--;
    return true;
  }