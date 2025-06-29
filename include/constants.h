
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "library_dependencies.h"
#include "secrets.h"


// Create an object of the class Bsec
extern Bsec iaqSensor;

extern String output;

extern Adafruit_CCS811 ccs;

//It is known Globally.The number of total devices we will use at the network.Set at constants.cpp
extern const int ESP32_TOTAL_DEVICES_NUMBER;

//It is known Globally.All the MAC addresses of the devices we have

extern const uint8_t ESP32_MAC_OF_WHITE_BREADBOARD[6];
extern const uint8_t ESP32_MAC_OF_TRANSPARENT_BREADBOARD[6];
extern const uint8_t ESP32_MAC_OF_YELLOW_BREADBOARD[6];

//It is known Globally.We gather all the above addresses into one array.Set at constants.cpp
extern const uint8_t *MAC_LIBRARY[];

extern uint8_t my_MAC[6];

extern int id;

extern const int idOfTheReceiver;

extern JsonDocument messageJSON;

extern const char* ssid ;
extern const char* password ;
extern String serverUrl ;

extern const uint8_t bsec_config_iaq[];
extern uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] ;
extern  const unsigned long  STATE_SAVE_PERIOD	;
extern uint16_t stateUpdateCounter ;

extern unsigned long CCS811_TIMER;
extern uint8_t minutes30HavePassed;

extern unsigned long CCS811_FREQUENCY;
extern unsigned int CCS811_EEPROM_SIZE;

extern unsigned long ENVIRONMENTAL_DATA_CCS811_FREQUENCY;
extern unsigned long ENVIRONMENTAL_DATA_CCS811_TIMER;


extern float temperature;
extern float humidity;


extern String sensorLocatedIntoDevice;

extern bool firstTimeAskingEnvironmentalData; //to know if we have asked the environmental data for the first time


extern String endpoint ;

extern String port;

extern  wifi_Information    selectedWIFI;
extern  String    selectedIP;

extern volatile bool dataReadyCCS811;

extern const uint CCS811_INT_PIN;

extern unsigned long timeSinceLastReading;

extern unsigned long timeSinceLastReadingMaxTolerance;

extern HTTPClient http;
extern WiFiClient wifiClient;
extern bool atSetup;

extern const char* ntpServer ;
extern const long  gmtOffset_sec ;//3600 seconds= 1 hour, so timezone 2 hours
extern const int   daylightOffset_sec ; //we would use 80000,but the divider reaches to 65536
extern float divided_frequency;//the timer will count as 0.1 milliseconds
extern float cofficient_to_reach_one_millisecond;
extern float millisecond_unit ;

extern int wifiIndex ;
extern int httpIndex ;
extern const int timeToConnectToWifi;
//set the timers
extern hw_timer_t *bme680Timer;

extern hw_timer_t *wifiTimer;

extern bool statusConnectedToWifi;
extern bool statusConnectedToServer;
extern unsigned long timeUntilNextReading;

extern JsonDocument doc;
extern JsonArray buffer;
extern const unsigned int maxBufferSize;
extern unsigned int currentNumberOfUnsendedData;
extern   bool  serverLostConnection;
#endif