



#ifndef customFunctions
#define customFunctions


#include "constants.h"
/// @brief Set the BME680 sensor if it exists using the adafruit library
/// @param 
/// @return True if the BME680 setup correctly, false if no bme680 exists in the board,the wiring is wrong or some
bool setBME680(Adafruit_BME680 *bme);
bool performBME680Reading(Adafruit_BME680 *bme,SensorMessage *myData,int id);
int chooseIDBasedOfMCA();
void readMacAddress();
void setReceiverMAC(const uint8_t targetMAC[6]);
void printMAC(const uint8_t myMAC[6]);
int chooseIDBasedOfMCA();
bool isTheReceiverESP32NOW(const uint8_t *mac_Address)();

void sendDataToSerial(const uint8_t *myData);
uint8_t* structToBytes(SensorMessage data);

//At the file customQueue.cpp
void initQueue(MessageQueue* q);
bool isQueueEmpty(const MessageQueue q);
bool isQueueFull(const MessageQueue q);
bool checkIfMessageWithSameIdExistsInQueue(const MessageQueue q,const SensorMessage msg );
bool enqueue(MessageQueue* q, SensorMessage msg);
bool dequeue(MessageQueue* q) ;

//At the sender code
void setupSender();
void loopSender();

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataRecv(const uint8_t *mac_addr,const uint8_t *incomingData, int len);

//At the requester code
void setupReceiver();
void loopReceiver();



#endif