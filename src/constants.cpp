#include "custom_headers.h"




const uint8_t *MAC_LIBRARY[] = {
    ESP32_MAC_OF_WHITE_BREADBOARD,
    ESP32_MAC_OF_TRANSPARENT_BREADBOARD,
    ESP32_MAC_OF_YELLOW_BREADBOARD
};

const int ESP32_TOTAL_DEVICES_NUMBER = 3;

uint8_t ESP32_MAC_OF_RECEIVER[6] = {0x0,0x0,0x0,0x0,0x0,0x0};
uint8_t my_MAC[6] = {0x0,0x0,0x0,0x0,0x0,0x0};
int id;
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
String serverUrl = "http://192.168.1.13:8080/";  // Your computer's local IP
// configuration for the bsec operation
const uint8_t bsec_config_iaq[] = {
  #include "config/generic_33v_3s_4d/bsec_iaq.txt"
};
 const unsigned long  STATE_SAVE_PERIOD	= 360 * 60 * 1000; // 360 minutes - 4 times a day
 uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};

//30minutes of css811 operation have passed to know it has been calibrated
uint8_t minutes30HavePassed = 0;

//Frequency of the reading of the CCS811 sensor
unsigned long CCS811_FREQUENCY = 3 * 1000;

unsigned long CCS811_TIMER = 0;

int CCS811_EEPROM_SIZE = sizeof(uint16_t); //so 2 bytes size

float roomTemperature;
float roomHumidity;

unsigned long ENVIRONMENTAL_DATA_CCS811_FREQUENCY = 60 * 60 * 1000; //1 hour
unsigned long ENVIRONMENTAL_DATA_CCS811_TIMER = 0; //timer for the environmental data