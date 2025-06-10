



#ifndef CUSTOM_FUNCTIONS_H
#define CUSTOM_FUNCTIONS_H

#include "library_dependencies.h"
#include "constants.h"


bool setupBME680();


bool setupCCS811();

bool loopSensor();

bool loopBME680();

bool loopCCS811();

void IRAM_ATTR ccs811Interrupt();

// Helper functions declarations for the bme680 bsec
bool checkIaqSensorStatus(bool atSetup);
//throw an infitive loop in case of an error;
void errLeds(void);
//flash leds.It is used as the python script cannot be accessed at the same time
void flashLeds();

void connectToWifiAndServer();

/// @brief We assign an ID to the device for identifying messages with particular devices without hardwriting a MAC address
///        into the code.
/// @param MAC_LIBRARY the array that contain the MAC addresses of all the devices that are going to be used
/// @return //if the MAC address exists, return an ID based of the position of the address at the MAC_LIBRARY array,
            //if the MAC address doesn't exists, return false.
int chooseIDBasedOfMAC(const uint8_t *MAC_LIBRARY[]);


void initializemessageJSON();



void loadBME680State(void);

void updateBME680State(void);

void checkIf30MinutesHavePassedCSS811();

void loadCCS811Baseline();

void updateCCS811Baseline();

void getEnvironmentalData();



void setupConnectionInformation();
void connectToWifiAndServer();
void createTheUrl(String IP);

void scanWiFiNetworks();

void listAvailableWiFiNetworks();

#endif