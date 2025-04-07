
#include "custom_headers.h"
#include "customFunctions.h"

bool createMessageQueue(MessageQueue *receiverQueue,const int sizeOfQueue) {
    /*
    Serial.print("Size of the queue will be ");
    Serial.println(sizeOfQueue);
    Serial.print("Size of the sensor Message bytes is ");
    Serial.println(sizeof(sensorMessage));

    //allocate the outer memory
  receiverQueue = (MessageQueue*) malloc(sizeof(MessageQueue));
  */
  if (receiverQueue == NULL) {
      perror("Failed to allocate outer structure");
      errLeds();
  }
  /*
  Serial.print("Size of the receiverQueue bytes is ");
  Serial.println(sizeof(*receiverQueue));
  */
  receiverQueue->messages= (sensorMessage*) malloc(sizeOfQueue * sizeof(sensorMessage));
  if (receiverQueue->messages == NULL) {
      Serial.println("Memory wasn't allocated");
      free(receiverQueue); //free previous memory allocated
      errLeds();
  } 
  /*
  Serial.print("Size of a queue message pointer in bytes ");
  Serial.println(sizeof((receiverQueue->messages)));

    Serial.print("Size of a queue message in bytes ");
    Serial.println(sizeof((*receiverQueue->messages)));

    Serial.print("Size of a queue is bytes ");
    Serial.println(sizeof((*receiverQueue)));
    */
    receiverQueue->maxSize = sizeOfQueue; //max size of queue
    receiverQueue->front = 0;
    receiverQueue->rear = 0;
    receiverQueue->currentSize = 0;
  
    return true;
  }
void freeQueueMemory(MessageQueue* receiverQueue){

  free(receiverQueue->messages);
  free(receiverQueue);
}  
void printQueueElements(MessageQueue* receiverQueue){
  Serial.println("The elements of queue are:");
  output = "maxSize"+ String(receiverQueue->maxSize);
  Serial.println(output);
  output = "front element" + String(receiverQueue->front);
  Serial.println(output);
  output = "rear element " + String(receiverQueue->rear);
  Serial.println(output);
  output = "currentSize" + String(receiverQueue->currentSize);
  Serial.println(output);
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
    // Serial.println("Queue is full");
    return true;
  }
  else{
    // Serial.println("Queue is not full");

    return false;
  }
}



bool insertMessageIntoQueue(MessageQueue* receiverQueue,sensorMessage message){
   // printQueueElements(receiverQueue);
    if (isQueueFull(receiverQueue)){
      // Serial.println("The Queue insertion was not succesful");
      return false;
    }
    //Insert the message,increase the rear flag one value and increase the current size.
    //We consider that the zero element at the start is the front element
    //As we use mod operator, the two counters are going back and forth into a circle of the MAXSIZE - 1 but the follow an incremental way.
    receiverQueue->messages[receiverQueue->rear] = message;
    // output = "Id of the new message is:" + String(receiverQueue->messages[receiverQueue->rear].id) +" and sensor type:" +String(receiverQueue->messages[receiverQueue->rear].sensor);

    receiverQueue->rear = (receiverQueue->rear + 1) % receiverQueue->maxSize;
    receiverQueue->currentSize++;
    //debug message
    // Serial.println(output);
    // Serial.println("The Queue insertion was succesful");
    return true;

  }
bool insertMessageIntoSerial(MessageQueue* receiverQueue,bool *messageInSerialPending,int *idOfLastMessageInsertedIntoSerial){
 // printQueueElements(receiverQueue);

    if (isQueueEmpty(receiverQueue)) {return false;}



    sensorMessage message;
    //Take the message,increase the front flag one value and decrease the current size
    message = receiverQueue->messages[receiverQueue->front];
    receiverQueue->front =(receiverQueue->front + 1) % receiverQueue->maxSize;
    receiverQueue->currentSize--;

     //give the necessary variables for the receiver loop
     *messageInSerialPending = true;
     *idOfLastMessageInsertedIntoSerial = message.id; 

    //debug message
    // output = "sensor:" + String(message.sensor )+ " id:" + String(message.id);
    // Serial.println(output);

    sendDataToSerial(message);

   
    
   
    return true;
  }