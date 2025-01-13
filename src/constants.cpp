#include <stdint.h>
#include "constants.h"

const uint8_t ESP32_MAC_OF_WHITE_BREADBOARD [6] = {0xE8,0x9F,0x6D,0xE8,0x66,0x4C };
const uint8_t ESP32_MAC_OF_TRANSPARENT_BREADBOARD[6] = {0xE8,0x9F,0x6D,0xE8,0xF7,0x0C};
const uint8_t ESP32_MAC_OF_YELLOW_BREADBOARD[6] = {0x44,0x17,0x93,0xFA,0xED,0x34};
uint8_t ESP32_MAC_OF_RECEIVER[6];

extern const int ESP32_ID_WHITE_BREADBOARD =  0 ;
extern const int ESP32_ID_TRANSPARENT_BREADBOARD = 1 ;
extern const int ESP32_ID_YELLOW_BREADBOARD = 2 ;


