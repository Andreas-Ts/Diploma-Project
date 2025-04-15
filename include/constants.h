
#ifndef constants
#define constants

#include "library_dependencies.h"
#include "enums.h"
#include "structures.h"


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

//It is known Globally.The MAC address of the device that will receive the information.Set at main.cpp 
extern uint8_t ESP32_MAC_OF_RECEIVER[6];


extern const int idOfTheReceiver;

//It is known only Locally.The MAC address of the particular device.Set at main.cpp 
extern uint8_t my_MAC[6];

//It is known Globally.We set the minimum number of bytes we need available to write something at Serial.Set at constants.cpp
extern int MINIMUM_BYTE_TO_WRITE_AT_SERIAL;

//It is known only Locally.The ID of the particular device.Set at main.cpp 
extern int id;

//It is known only Locally.Flag for stopping the device if something happens.Set at false at constants.cpp
extern bool is_Everything_Ok;

//It is known Globally.Sea level pressure, propably unnecessary.Set at constants.cpp
extern const float SEALEVELPRESSURE_HPA;

//It is known only Locally.It is the information of the peers.Set at constants.cpp
extern esp_now_peer_info_t peerInfo;

//It is known only Locally.Flag for checking the state of the response
extern Setting settingOfSensor;

extern bool waitingResponseSerial;

//Frequency of messages we want to have as minimum for normal workflow
extern const float frequencyMinimum;

//Frequency when a not normal behavior occurs,as a full queue.
extern const float frequencyWhenFailureOccurs;


extern recognized_Sensor sensorLocatedIntoDevice;

//It is known only Locally.Time since we send the last message in order not to overflow the receiver.
extern  unsigned long timeLastMessageWasSend;

//It is known Globally.Max waiting time for a device to take back a confirmation of sucessuful or not writing from the receiver.
//Set currently at 5 minutes.Set at constants.cpp
extern const  unsigned long maxWaitingTime;

//It is known Globally.Max waiting time for a device to take back a confirmation of sucessuful or not writing from the python
//set at 30 second
extern const  unsigned long maxWaitingTimeSerial;

//The id that the last message inserted into the serial had

extern int  idOfLastMessageInsertedIntoSerial;

//It is known only Locally.Time since we send the last message in order not to overflow the receiver.
extern  unsigned long timeLastMessageWasSendInSerial;

//Check if we have a message pending at Serial

extern bool messageInSerialPending;


#endif