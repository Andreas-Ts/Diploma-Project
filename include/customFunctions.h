



#ifndef customFunctions
#define customFunctions


#include "constants.h"

void setBME680(Adafruit_BME680 *bme);
void performBME680Reading(Adafruit_BME680 *bme,SensorMessage *myData);
int chooseIDBasedOfMCA();
void setReceiverMAC(const uint8_t targetMAC[6]);
int chooseIDBasedOfMCA();
uint8_t amITheReceiverESP32NOW();
char* makeMACAdressEasilyPrintable(const uint8_t mac_addr[6]);
void setupSender();
void loopSender();
void sendDataToSerial(byte *pointerToSensorMessage,int lengthOfSensorMessage);
#endif