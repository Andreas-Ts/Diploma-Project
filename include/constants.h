
#ifndef constants
#define constants
#include "library_dependencies.h"


//The number of total devices we will use at the network.Set at constants.cpp
extern const int ESP32_TOTAL_DEVICES_NUMBER;

//All the MAC addresses of the devices we have

extern const uint8_t ESP32_MAC_OF_WHITE_BREADBOARD[6];
extern const uint8_t ESP32_MAC_OF_TRANSPARENT_BREADBOARD[6];
extern const uint8_t ESP32_MAC_OF_YELLOW_BREADBOARD[6];

//We gather all the above addresses into one array at constants.cpp
extern const uint8_t *MAC_LIBRARY[];

//The MAC address of the device that will receive the information 
extern uint8_t ESP32_MAC_OF_RECEIVER[6];
extern uint8_t my_MAC[6];


extern const int ESP32_ID_WHITE_BREADBOARD;
extern const int ESP32_ID_TRANSPARENT_BREADBOARD;
extern const int ESP32_ID_YELLOW_BREADBOARD;

typedef struct SensorMessage{

  int id; /*Id of the microcontroller having the sensor*/
  
  float temperature; /*Temperature (Celsius) assigned after calling performReading() or*/

  uint32_t pressure; /*Pressure (Pascals) assigned after calling performReading() or endReading()*/

  float humidity; /*Humidity (RH %) assigned after calling performReading() or endReading()*/

  uint32_t gas_resistance;/*Gas resistor (ohms) assigned after calling performReading() or endReading()*/
  
  float altitude; /*Calculates the altitude (in meters).Reads the current atmostpheric pressure (in hPa) from the sensor and
  * calculates via the provided sea-level pressure (in hPa).*/

}SensorMessage;

extern SensorMessage myData;


typedef struct MessageQueue {
  SensorMessage *messages;  // Δείκτης προς δυναμικά δεσμευμένη μνήμη
  int maxSize;              // Μέγιστο μέγεθος ουράς
  int front;                 // Index πρώτου στοιχείου
  int rear;                  // Index επόμενου στοιχείου
  int currentSize;                 // Τρέχον πλήθος στοιχείων
} MessageQueue;

typedef struct ResponseMessageFromPython {
  int id;
  bool writtenSuccesufully;
}ResponseMessageFromPython;

typedef struct ErrorMessage{
  int id;
  char* error;
}ErrorMessage;

extern int id;
extern Adafruit_BME680 bme; 

extern const uint8_t BME_SCK;
extern const uint8_t BME_MISO;
extern const uint8_t BME_MOSI;
extern const uint8_t BME_CS;


extern bool is_Everything_Ok;

extern const float SEALEVELPRESSURE_HPA;

extern esp_now_peer_info_t peerInfo;

extern bool waitingResponse;

extern unsigned long timeLastMessageWasSend;
extern unsigned long maxWaitingTime;

extern MessageQueue* receiverQueue;
#endif