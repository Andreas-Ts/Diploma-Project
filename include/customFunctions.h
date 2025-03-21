



#ifndef customFunctions
#define customFunctions

#include "library_dependencies.h"
#include "constants.h"


bool setupBME680();


bool setupCCS811();

void loopSensor(sensorMessage *message);

void loopBME680(sensorMessage *message);


// Helper functions declarations for the bme680 bsec
bool checkIaqSensorStatus(bool atSetup);
void errLeds(void);


/// @brief Set the BME680 sensor if it exists using the adafruit library
/// @param bme Adafruit_BME680 Class for both I2C and SPI usage.
///     Wraps the Bosch library for Arduino usage 
/// @return True if the BME680 setup correctly,
/// false if no bme680 exists in the board,the wiring is wrong,some other problem occured or BME680 doesn't exists into the breadboard.
//bool setBME680(Adafruit_BME680 *bme);


/// @brief Set the BME680 if exists
/// @param bme Adafruit_BME680 Class for both I2C and SPI usage.
///     Wraps the Bosch library for Arduino usage
/// @param myData the data read from the bme680 sensor that get returned as a parameter reference
/// @param id The id of the device that had read the bme680 sensor
/// @return Return true if the reading of the bme680 executed correctly, false if any error occured
//bool performBME680Reading(Adafruit_BME680 *bme,SensorMessage *myData,int id);


/// @brief We assign an ID to the device for identifying messages with particular devices without hardwriting a MAC address
///        into the code.
/// @param MAC_LIBRARY the array that contain the MAC addresses of all the devices that are going to be used
/// @return //if the MAC address exists, return an ID based of the position of the address at the MAC_LIBRARY array,
            //if the MAC address doesn't exists, return false.
int chooseIDBasedOfMAC(const uint8_t *MAC_LIBRARY[]);


/// @brief Read the MAC address of the esp32
/// @return return true if MAC was found, false if was not found
bool readMacAddress();


/// @brief choose a particular esp32 MAC address to be the receiver esp32 device
/// @param MAC_LIBRARY the array that has all the MAC addresses.
/// @param library_position the position that the MAC address is located into the library
void setReceiverMAC(const uint8_t *MAC_LIBRARY[],const int library_position);


/// @brief print the MAC address into a more easily readable form
/// @param MAC_ADDRESS the MAC address we want to print
void printMAC(const uint8_t MAC_ADDRESS[6]);

/// @brief As the code for the esp32 that gets all the information of the several devices is different from the 
///        simple node devices, we check the device in order to direct it to different code.
/// @param MAC_ADDRESS the MAC address of the device we want to check if it the receiver device
/// @return true if it is the receiver, false if it is not. 
bool isTheReceiverESP32NOW(const uint8_t MAC_ADDRESS[6]);

/// @brief We sent data from the receiver to the personal computer that is connected through a type b cable
/// @param dataToBeSentToSerial the data that is going to be sent from the esp32 device to the computer. 
void sendDataToSerial(const uint8_t *dataToBeSentToSerial);

/// @brief We still work on this 
/// @param data 
/// @return 
uint8_t* structToBytes(sensorMessage data);

/// @brief In order for the sender devices not to overflow the receiver device, we stop the device for reading
///       new reading and sending new messages until it has taken a confirmation that the message has been read.
/// @param flag it uses the Enum Setting sets the flag waitingResponse either to true sending the variable WAITING
///        or false with the word FINISHED
/// @param waitingResponse  waitingResponse constant of the device
/// @param timeLastMessageWasSend pointer to the timeLastMessageWasSend constant of the device
void setTimerAndFlag(Setting setting,bool *waitingResponse,unsigned long *timeLastMessageWasSend);

/// @brief We check if the time the device has spent waiting is over the thereshold set 
/// by maxWaitingTime, constant of the device
/// @param timeLastMessageWasSend the timeLastMessageWasSend constant of the device 
/// @return true if the time passed exceeded the thereshold, false if not.
bool checkForInactivityOverThreshold(unsigned long *timeLastMessageWasSend,unsigned long threshold);

/// @brief we send check if some minimum time have passed before sending a message
/// @param timeLastMessageWasSend we initialize it at loop sensor
/// @return 
bool isTimeToSendMessage(const unsigned long timeLastMessageWasSend);

/// @brief Read the response of the python script when it has succesfully or not read the input
void ReadFromSerial();

ResponseMessageFromReceiver createResponseFromReceiver(const int sensorMessage,bool writtenIntoQueue,bool writtenIntoPython);
//////////////////////////////////////////////
//At the file customQueue.cpp
////////////////////////////////////////////

/// @brief We create the Queue that it will handle the sensor messages at the receiver device. 
/// We only create it at the receiver device as the esp32 has limited space
/// @param sizeOfQueue The maximum size of the message queue.The ESP32_TOTAL_DEVICES_NUMBER constant will set it
/// @return the created queue
MessageQueue* createMessageQueue(const int sizeOfQueue);

///@brief Release the dynamically allocated memory we got for the queue of the receiver device
///@param queue the queue that handles the messages
void freeQueueMemory(MessageQueue* queue);

/// @brief We check if the queue is empty
/// @param queue the queue that handles the messages
/// @return true if queue is empty, false if not empty
bool isQueueEmpty(MessageQueue* queue);

/// @brief We check if the queue is full
/// @param queue the queue that handles the messages
/// @return true if queue is full, false if not full
bool isQueueFull(MessageQueue* queue);

/// @brief Insert a new message into the back of the queue
/// @param queue pointer to the queue that handles the messages
/// @param message the message we want to insert into the queue
/// @return true if it the insertion was successful,false if not (probably because of full queue)
bool insertMessageIntoQueue(MessageQueue* queue,SensorMessage message);

/// @brief Take the front message of the queue and send it into the serial calbe
/// @param queue the queue that handles the messages
/// @return true if it the insertion was successful,false if not (probably because of empty queue) 
bool insertMessageIntoSerial(MessageQueue* queue);

///////////////////////////
//The esp32 now related functions
///////////////////////////

///////////////////////////
//At the sender code
///////////////////////////

/// @brief The setup function of the sender devices (not the device that gathers all the other sensor reading)
/// It will have the esp_now_register_send_cb(callback function) and esp_now_register_recv_cb(callback function);
void setupSender();
/// @brief The loop function of the sender devices (not the device that gathers all the other sensor reading)
void loopSender();

/// @brief The callback function which we insert at esp_now_register_send_cb(), which it is called after we have use esp_now_send() to send a message 
///        Here the message will be information we get from the device's sensor.
/// @param mac_addr The MAC address at which we sent the message
/// @param status Is either ESP_NOW_SEND_SUCCESS = 0 or ESP_NOW_SEND_FAIL
void OnDataSentAsSender(const uint8_t *mac_addr, esp_now_send_status_t status);

/// @brief The callback function which we insert at esp_now_register_recv_cb(),which it is called when a device receives a message.
///        Here the message will be the confirmation that the receiver wrote the data into Serial.write and received the confirmation
///        that the message had been written into the computer.
/// @param mac_addr The MAC address of the receiver
/// @param incomingData A struct message containing confirmation of sucessufully or not writing the information into storage 
/// @param len the size of the incomingData
void OnDataRecvAsSender(const uint8_t *mac_addr,const uint8_t *incomingData, int len) ;

///////////////////////////
//At the requester code
///////////////////////////

/// @brief The setup function of the receiver device (the one device that gathers all the other sensor reading).
///        It will have the esp_now_register_send_cb(callback function) and esp_now_register_recv_cb(callback function);
void setupReceiver();
/// @brief The loop function of the receiver device (the one device that gathers all the other sensor reading)
void loopReceiver();

/// @brief The callback function which we insert at esp_now_register_send_cb() after we have use esp_now_send() to send a message 
//// @param mac_addr The MAC address at which we sent the message
/// @param status Is either ESP_NOW_SEND_SUCCESS = 0 or ESP_NOW_SEND_FAIL
void OnDataSentAsReceiver(const uint8_t *mac_addr, esp_now_send_status_t status);

/// @brief The callback function which we insert at esp_now_register_recv_cb(),which it is called when a device receives a message.
/// @param mac_addr The MAC address from which we receive the message
/// @param incomingData A struct message containing a sample of a device's sensor,distuinguished by its id.
/// @param len the size of the incomingData
void OnDataRecvAsReceiver(const uint8_t *mac_addr,const uint8_t *incomingData, int len);


#endif