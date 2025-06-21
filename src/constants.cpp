#include "custom_headers.h"

const uint8_t ESP32_MAC_OF_WHITE_BREADBOARD [6] = {0xE8,0x9F,0x6D,0xE8,0x66,0x4C };
const uint8_t ESP32_MAC_OF_TRANSPARENT_BREADBOARD[6] = {0xE8,0x9F,0x6D,0xE8,0xF7,0x0C};
const uint8_t ESP32_MAC_OF_YELLOW_BREADBOARD[6] = {0x44,0x17,0x93,0xFA,0xED,0x34};

//Part of previous implementation to use esp-now. Now it is used to assign the id of the device based of its position to the array
const uint8_t *MAC_LIBRARY[] = {
    ESP32_MAC_OF_WHITE_BREADBOARD,
    ESP32_MAC_OF_TRANSPARENT_BREADBOARD,
    ESP32_MAC_OF_YELLOW_BREADBOARD
};
uint8_t my_MAC[6] = {0x0,0x0,0x0,0x0,0x0,0x0};

int id;

//all the mac has the same size
const int ESP32_TOTAL_DEVICES_NUMBER = sizeof(MAC_LIBRARY)/sizeof(MAC_LIBRARY[0]);



//the object that handles the BSEC functions
Bsec iaqSensor;

String output;
Adafruit_CCS811 ccs;
String sensorLocatedIntoDevice = "NO_KNOWN_SENSOR";

JsonDocument messageJSON;


// Server URL


String port =":8080"; // Port number for the server
String endpoint = "/timeSeriesEndpoints/";
String serverUrl  ;  // Your computer's local IP


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

unsigned int CCS811_EEPROM_SIZE = sizeof(uint16_t) + 1; //so 2 bytes size

float temperature;
float humidity;

unsigned long ENVIRONMENTAL_DATA_CCS811_FREQUENCY = 30 * 60 * 1000; //30 minutes
unsigned long ENVIRONMENTAL_DATA_CCS811_TIMER = 0; //timer for the environmental data

uint16_t stateUpdateCounter = 0 ; //counter for the state update of the CCS811 sensor and the BME680 sensor

bool firstTimeAskingEnvironmentalData = true; //to know if we have asked the environmental data for the first time


wifi_Information    selectedWIFI;
String    selectedIP;

 wifi_Information* connectionInformation = nullptr;

volatile bool dataReadyCCS811 = false;


unsigned long timeSinceLastReading;

unsigned long timeSinceLastReadingMaxTolerance = 30 * 1000; //30 seconds


HTTPClient http;

bool atSetup = true;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600 * 2;//3600 seconds= 1 hour, so timezone 2 hours
const int   daylightOffset_sec = 3200;
const uint32_t frequency_divider = 8000; //we would use 80000,but the divider reaches to 65536
float divided_frequency = (8 *10^7) /frequency_divider;//the timer will count as 0.1 milliseconds
float cofficient_to_reach_one_millisecond = (10^-3)/divided_frequency;
float millisecond_unit = cofficient_to_reach_one_millisecond * divided_frequency;

unsigned int wifiIndex = 0;
unsigned int httpIndex = 0;
int timeToConnectToWifi = 5000;
//set the timers
hw_timer_t *bme680Timer = timerBegin(0,frequency_divider,true);

hw_timer_t *wifiTimer = timerBegin(1,frequency_divider,true);


bool statusConnectedToWifi = false;
bool statusConnectedToServer = false;

unsigned long timeUntilNextReaind =0;