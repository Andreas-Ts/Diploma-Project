#include "custom_headers.h"




const uint8_t *MAC_LIBRARY[] = {
    ESP32_MAC_OF_WHITE_BREADBOARD,
    ESP32_MAC_OF_TRANSPARENT_BREADBOARD,
    ESP32_MAC_OF_YELLOW_BREADBOARD
};

const int ESP32_TOTAL_DEVICES_NUMBER = 3;

uint8_t ESP32_MAC_OF_RECEIVER[6] = {0x0,0x0,0x0,0x0,0x0,0x0};
uint8_t my_MAC[6] = {0x0,0x0,0x0,0x0,0x0,0x0};

const uint8_t ESP32_MAC_OF_WHITE_BREADBOARD [6] = {0xE8,0x9F,0x6D,0xE8,0x66,0x4C };
const uint8_t ESP32_MAC_OF_TRANSPARENT_BREADBOARD[6] = {0xE8,0x9F,0x6D,0xE8,0xF7,0x0C};
const uint8_t ESP32_MAC_OF_YELLOW_BREADBOARD[6] = {0x44,0x17,0x93,0xFA,0xED,0x34};

const int idOfTheReceiver = 2;


//const uint8_t BME_SCK = 39;
//const uint8_t BME_SDI = 42;
 
int MINIMUM_BYTE_TO_WRITE_AT_SERIAL = sizeof(sensorMessage);

//const float SEALEVELPRESSURE_HPA = 1030;

Bsec iaqSensor;
String output;
Adafruit_CCS811 ccs;

esp_now_peer_info_t peerInfo;

int id;

bool is_Everything_Ok = true;
Setting settingOfSensor = NO_WAIT;

unsigned long timeLastMessageWasSend = 0;
unsigned const long timeLastMessageWasSendSerial = 0;
unsigned const long maxWaitingTimeSerial = 1000 * 120; //30 seconds
unsigned const long maxWaitingTime = 60000 * 10; //60000 milliseconds is 1 minute, so 1 multiply them by 10

recognized_Sensor sensorLocatedIntoDevice = NO_KNOWN_SENSOR;

sensorMessage message;
//Frequency of messages we want to have as minimum for normal workflow
const float frequencyMinimum = 1.0;

//Frequency when a not normal behavior occurs,as a full queue.
const float frequencyWhenFailureOccurs = 10.0;


//at first is false as we don't have sent any message 

bool messageInSerialPending = false;

int idOfLastMessageInsertedIntoSerial;

MessageQueue receiverQueue;
