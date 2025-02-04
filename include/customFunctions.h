



#ifndef customFunctions
#define customFunctions


#include "constants.h"

void setBME680(Adafruit_BME680 *bme);
void performBME680Reading(Adafruit_BME680 *bme,SensorMessage *myData,int id);
int chooseIDBasedOfMCA();
void readMacAddress();
void setReceiverMAC(const uint8_t targetMAC[6]);
int chooseIDBasedOfMCA();
uint8_t amITheReceiverESP32NOW();
char* makeMACAdressEasilyPrintable(const uint8_t mac_addr[6]);
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataRecv(const uint8_t *mac_addr,const uint8_t *incomingData, int len);

void setupSender();
void loopSender();
void setupReceiver();
void loopReceiver();

void sendDataToSerial(byte *pointerToSensorMessage,int lengthOfSensorMessage);
#endif