



#ifndef customFunctions
#define customFunctions


#include "constants.h"

bool setBME680(Adafruit_BME680 *bme);
bool performBME680Reading(Adafruit_BME680 *bme,SensorMessage *myData,int id);
int chooseIDBasedOfMCA();
void readMacAddress();
void setReceiverMAC(const uint8_t targetMAC[6]);
void printMAC(const uint8_t myMAC[6]);
int chooseIDBasedOfMCA();
uint8_t amITheReceiverESP32NOW();
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataRecv(const uint8_t *mac_addr,const uint8_t *incomingData, int len);
void sendDataToSerial(const uint8_t *myData);
uint8_t* structToBytes(SensorMessage data);
#endif