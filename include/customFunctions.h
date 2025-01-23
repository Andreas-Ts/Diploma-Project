



#ifndef customFunctions
#define customFunctions

#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include "constants.h"

void setBME680(Adafruit_BME680 *bme);
void performBME680Reading(Adafruit_BME680 *bme,SensorMessage *myData);
int chooseIDBasedOfMCA();
void setReceiverMAC(const uint8_t targetMAC[6]);
int chooseIDBasedOfMCA();
uint8_t amITheReceiverESP32NOW();
void setupSender();
void loopSender();
#endif