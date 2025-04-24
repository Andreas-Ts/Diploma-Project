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

Bsec iaqSensor;
String output;
Adafruit_CCS811 ccs;
String sensorLocatedIntoDevice = "NO_KNOWN_SENSOR";

JsonDocument messageJSON;

// Replace with your network credentials
const char* ssid = "COSMOTE-609943";
const char* password = "n9e6bmreeax2x7u3";

// Server URL
const char* serverUrl = "http://192.168.1.5:8080";  // Your computer's local IP
// configuration for the bsec operation
const uint8_t bsec_config_iaq[] = {
  #include "config/generic_33v_3s_4d/bsec_iaq.txt"
};
 const unsigned long  STATE_SAVE_PERIOD	= 360 * 60 * 1000; // 360 minutes - 4 times a day
 uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};

//30minutes of css811 operation have passed to know it has been calibrated
uint8_t minutes30HavePassed = 0;

const int idOfTheReceiver = 2;


//const uint8_t BME_SCK = 39;
//const uint8_t BME_SDI = 42;
 
int MINIMUM_BYTE_TO_WRITE_AT_SERIAL = sizeof(sensorMessage);

//const float SEALEVELPRESSURE_HPA = 1030;
uint16_t stateUpdateCounter = 0;

unsigned long CSS811_TIMER=0;

esp_now_peer_info_t peerInfo;

int id;

bool is_Everything_Ok = true;
Setting settingOfSensor = NO_WAIT;

unsigned long timeLastMessageWasSend = 0;
unsigned const long timeLastMessageWasSendSerial = 0;
unsigned const long maxWaitingTimeSerial = 60000 * 1; //30 seconds
unsigned const long maxWaitingTime = 60000 * 1; //60000 milliseconds is 1 minute, so 1 multiply them by 10


sensorMessage message;
//Frequency of messages we want to have as minimum for normal workflow
const float frequencyMinimum = 1.1 *1000;

//Frequency when a not normal behavior occurs,as a full queue.
const float frequencyWhenFailureOccurs = 6.0 *1000;


//at first is false as we don't have sent any message 

bool messageInSerialPending = false;

int idOfLastMessageInsertedIntoSerial;

unsigned long timeLastMessageWasSendInSerial;

MessageQueue receiverQueue;
