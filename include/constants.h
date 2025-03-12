
#ifndef constants
#define constants
#include "library_dependencies.h"


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

//It is known only Locally.The MAC address of the particular device.Set at main.cpp 
extern uint8_t my_MAC[6];

//It is known Globally.We set the minimum number of bytes we need available to write something at Serial.Set at constants.cpp
extern int MINIMUM_BYTE_TO_WRITE_AT_SERIAL;

//The structure of message that will contain an instance of sensor's information.
typedef struct SensorMessage{

  int id; /*Id of the microcontroller having the sensor*/
  
  float temperature; /*Temperature (Celsius) assigned after calling performReading() or*/

  uint32_t pressure; /*Pressure (Pascals) assigned after calling performReading() or endReading()*/

  float humidity; /*Humidity (RH %) assigned after calling performReading() or endReading()*/

  uint32_t gas_resistance;/*Gas resistor (ohms) assigned after calling performReading() or endReading()*/
  
  float altitude; /*Calculates the altitude (in meters).Reads the current atmostpheric pressure (in hPa) from the sensor and
  * calculates via the provided sea-level pressure (in hPa).*/

}SensorMessage;

//Set at constants.cpp only for the purpose of avoiding duplicates.
extern SensorMessage sensorMessage;

//The queue at receiver device that keeps the messages containing the sensors information of all devices, even itself
typedef struct MessageQueue {
  SensorMessage *messages;  // Δείκτης προς δυναμικά δεσμευμένη μνήμη
  int maxSize;              // Μέγιστο μέγεθος ουράς
  int front;                 // Index πρώτου στοιχείου
  int rear;                  // Index επόμενου στοιχείου
  int currentSize;                 // Τρέχον πλήθος στοιχείων
} MessageQueue;

//Set at customQueue.cpp only for the purpose of avoiding taking too much space
extern MessageQueue* receiverQueue;

//The response message that the receiver sends to the sender devices after it read the confirmation back from the python script
typedef struct ResponseMessageFromReceiver {
  int id;
  bool writtenIntoQueue;
  bool writtenSuccessfully;
}ResponseMessageFromReceiver;


//Just in case
typedef struct ErrorMessage{
  int id;
  char* error;
}ErrorMessage;

//It is known only Locally.The ID of the particular device.Set at main.cpp 
extern int id;

//Adafruit_BME680 Class for both I2C and SPI usage. Wraps the Bosch library for Arduino usage 
extern Adafruit_BME680 bme; 

//Variables for the bme object.Propably are useless
extern const uint8_t BME_SCK;
extern const uint8_t BME_MISO;
extern const uint8_t BME_MOSI;
extern const uint8_t BME_CS;

//It is known only Locally.Flag for stopping the device if something happens.Set at false at constants.cpp
extern bool is_Everything_Ok;

//It is known Globally.Sea level pressure, propably unnecessary.Set at constants.cpp
extern const float SEALEVELPRESSURE_HPA;

//It is known only Locally.It is the information of the peers.Set at constants.cpp
extern esp_now_peer_info_t peerInfo;

//It is known only Locally.Flag for stopping the device for reading sensor readings.Set at false at constants.cpp
extern bool waitingResponse;

extern bool waitingResponseSerial;

//Enum variable for the parameter of the function that activates the waitingResponse
enum Setting {
  WAITING,
  FINISHED_SUCCESSFULLY,
  FINISHED_STANDBY
};

//It is known only Locally.Time since last Sensor Message was sent. Set to zero 0 at constants.cpp
extern unsigned long timeLastMessageWasSend;

//It is known Globally.Max waiting time for a device to take back a confirmation of sucessuful or not writing from the receiver.
//Set currently at 5 minutes.Set at constants.cpp
extern unsigned long maxWaitingTime;

extern unsigned long timeLastMessageWasSendSerial;

extern unsigned long maxWaitingTimeSerial;

#endif