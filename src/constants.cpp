#include "constants.h"
#include "customFunctions.h"


const int ESP32_TOTAL_DEVICES_NUMBER = 3;

uint8_t ESP32_MAC_OF_RECEIVER[6] = {0x0,0x0,0x0,0x0,0x0,0x0};
uint8_t my_MAC[6] = {0x0,0x0,0x0,0x0,0x0,0x0};

const uint8_t ESP32_MAC_OF_WHITE_BREADBOARD [6] = {0xE8,0x9F,0x6D,0xE8,0x66,0x4C };
const uint8_t ESP32_MAC_OF_TRANSPARENT_BREADBOARD[6] = {0xE8,0x9F,0x6D,0xE8,0xF7,0x0C};
const uint8_t ESP32_MAC_OF_YELLOW_BREADBOARD[6] = {0x44,0x17,0x93,0xFA,0xED,0x34};

const uint8_t *MAC_LIBRARY[ESP32_TOTAL_DEVICES_NUMBER] = {
    ESP32_MAC_OF_WHITE_BREADBOARD,
    ESP32_MAC_OF_TRANSPARENT_BREADBOARD,
    ESP32_MAC_OF_YELLOW_BREADBOARD
};

const uint8_t BME_SCK = 13;
const uint8_t BME_MISO = 12;
const uint8_t BME_MOSI = 11;
const uint8_t BME_CS = 10;
 
const float SEALEVELPRESSURE_HPA = 1013.25;

esp_now_peer_info_t peerInfo;
SensorMessage myData;
uint8_t id;