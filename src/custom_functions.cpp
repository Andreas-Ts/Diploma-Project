
#include "custom_headers.h"
#include "customFunctions.h"

bool readMacAddress(){
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, my_MAC);
  if (ret == ESP_OK) {
   //  printMAC(my_MAC);
     return true;
  } else {
    Serial.println("Failed to read MAC address");
    return false;
  }
}

void setReceiverMAC(const uint8_t *MAC_LIBRARY[],const int library_position){
  
  memcpy(ESP32_MAC_OF_RECEIVER, MAC_LIBRARY[library_position], 6);
}



bool setupBME680(){
  
  iaqSensor.begin(BME68X_I2C_ADDR_HIGH, Wire);
  output = "\nBSEC library version " + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
 // Serial.println(output);
  //Perhaps the bme680 doesn't exist so we return false
  if (checkIaqSensorStatus(true) == false){
    return false;
  }

  bsec_virtual_sensor_t sensorList[13] = {
    BSEC_OUTPUT_IAQ,
    BSEC_OUTPUT_STATIC_IAQ,
    BSEC_OUTPUT_CO2_EQUIVALENT,
    BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    BSEC_OUTPUT_RAW_TEMPERATURE,
    BSEC_OUTPUT_RAW_PRESSURE,
    BSEC_OUTPUT_RAW_HUMIDITY,
    BSEC_OUTPUT_RAW_GAS,
    BSEC_OUTPUT_STABILIZATION_STATUS,
    BSEC_OUTPUT_RUN_IN_STATUS,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
    BSEC_OUTPUT_GAS_PERCENTAGE
  };

  iaqSensor.updateSubscription(sensorList, 13, BSEC_SAMPLE_RATE_CONT);
  checkIaqSensorStatus(false);

  // Print the header
  output = "Timestamp [ms], IAQ, IAQ accuracy, Static IAQ, CO2 equivalent, breath VOC equivalent, raw temp[°C], pressure [hPa], raw relative humidity [%], gas [Ohm], Stab Status, run in status, comp temp[°C], comp humidity [%], gas percentage";
  //Serial.println(output);
  return true;
}



bool setupCCS811(){
  //Serial.println("CCS811 test");

  if(!ccs.begin()){
    //Serial.println("Failed to start sensor! Please check your wiring.");
    return false;
  }

  // Wait for the sensor to be ready
  while(!ccs.available());
  return true;
}

void loopSensor(sensorMessage *message){
    if (message->sensor == BME680){
       loopBME680(message);
    }
    if (message->sensor == CSS811){
       loopCCS811(message);
    }
    flashLeds();
    settingOfSensor = WAIT_FOR_RESPONSE;
}

void loopBME680(sensorMessage *message){
  if (iaqSensor.run()) { // If new data is available
   message->informationFromSensor.sensorBME680.iaq  = iaqSensor.iaq;
   message->informationFromSensor.sensorBME680.iaqAccuracy  = iaqSensor.iaqAccuracy;
   message->informationFromSensor.sensorBME680.staticIaq  = iaqSensor.staticIaq;
   message->informationFromSensor.sensorBME680.co2Equivalent  = iaqSensor.co2Equivalent;
   message->informationFromSensor.sensorBME680.breathVocEquivalent  = iaqSensor.breathVocEquivalent;
   message->informationFromSensor.sensorBME680.rawTemperature  = iaqSensor.rawTemperature;
   message->informationFromSensor.sensorBME680.pressure  = iaqSensor.pressure;
   message->informationFromSensor.sensorBME680.rawHumidity  = iaqSensor.rawHumidity;
   message->informationFromSensor.sensorBME680.gasResistance  = iaqSensor.gasResistance;
   message->informationFromSensor.sensorBME680.stabStatus  = iaqSensor.stabStatus;
   message->informationFromSensor.sensorBME680.runInStatus  = iaqSensor.runInStatus;
   message->informationFromSensor.sensorBME680.temperature  = iaqSensor.temperature;
   message->informationFromSensor.sensorBME680.humidity  = iaqSensor.humidity;
   message->informationFromSensor.sensorBME680.gasPercentage  = iaqSensor.gasPercentage;
  } 
  checkIaqSensorStatus(false);
}

void loopCCS811(sensorMessage *message){
  if(ccs.available()){
    if(!ccs.readData()){
      message->informationFromSensor.sensorCSS811.eCO2 = ccs.geteCO2();
      message->informationFromSensor.sensorCSS811.TVOC = ccs.getTVOC();
    }
    else{
      Serial.println("Error at CSS811");
      errLeds();
    }
  }
}

// if bool atSetup=false, then we will terminate the loop of the device

bool checkIaqSensorStatus(bool atSetup)
{
  bool errorAppeared = false;

  if (iaqSensor.bsecStatus != BSEC_OK) {
    if (iaqSensor.bsecStatus < BSEC_OK) {
      output = "BSEC error code : " + String(iaqSensor.bsecStatus);
      Serial.println(output);
      bool errorAppeared = true;
      }
     else {
      output = "BSEC warning code : " + String(iaqSensor.bsecStatus);
      Serial.println(output);
    }
  }
 
  if (iaqSensor.bme68xStatus != BME68X_OK) {
    if (iaqSensor.bme68xStatus < BME68X_OK) {
      output = "BME68X error code : " + String(iaqSensor.bme68xStatus);
      Serial.println(output);
      bool errorAppeared = true;
    } else {
      output = "BME68X warning code : " + String(iaqSensor.bme68xStatus);
      Serial.println(output);
    }
  }

  if (errorAppeared == false){
    //Serial.println("Everything from BME680 is ok.");
    return true;
  }
  else{
    if (atSetup == true){
      return false;
    }
    else{
      Serial.println("Some error occured");
      errLeds();
      return false;

    }
}
}

int chooseIDBasedOfMAC(const uint8_t *MAC_LIBRARY[]){
  int id;
  esp_err_t  res= esp_wifi_get_mac(WIFI_IF_STA,my_MAC);
  if (res != ESP_OK){
     Serial.println("Failed to read MAC address");
     return -1;
  }
  
  int position=0;
  for ( ;  position < ESP32_TOTAL_DEVICES_NUMBER; position++ ){
    if (! memcmp(MAC_LIBRARY[position],my_MAC,sizeof(my_MAC))){
        return position;
    }
  }

  if (position >= ESP32_TOTAL_DEVICES_NUMBER){
       Serial.println("Failed to find a saved MAC address");
        return -1;
  }
  //just a safe check
  return -1;
}


//The memcmp returns 0 when both of the data are equal
bool isTheReceiverESP32NOW(const uint8_t *MAC_ADDRESS){
  if (memcmp(ESP32_MAC_OF_RECEIVER,MAC_ADDRESS,6) == 0) {
    return true;
  }  
  else {
    return false;
  }
}


//Get the size of the structs we have inside the union that repressents the payload of each sensor
int getByteSizeOfTypeOfSensor(const recognized_Sensor sensor){
  int byteSize=0;
  switch (sensor){
    case BME680:
      byteSize = sizeof(informationFromBME680);
      break;
    case CSS811: 
    byteSize = sizeof(informationFromCSS811);
      break; 
    default:
      Serial.println("Sensor aren't being recognized");
      errLeds();
  }
  return byteSize;
}

bool sendDataToSerial(const sensorMessage messageToBeInsertedToSerial){
 
    unsigned int bytesSend = 0;
    int sizeOfPayload = getByteSizeOfTypeOfSensor(messageToBeInsertedToSerial.sensor);



    uint8_t *dataToBeSentToSerial = (uint8_t*) malloc(sizeof(sensorMessage));
    if (dataToBeSentToSerial == NULL){
      Serial.println("Error: malloc failed");
      errLeds();
      return false;
    }
    uint8_t *dataSent= (uint8_t*) malloc(8);//allocate 8 bytes

    memcpy(dataToBeSentToSerial,&messageToBeInsertedToSerial,sizeof(messageToBeInsertedToSerial));
    if (dataSent == NULL){
      Serial.println("Error: malloc failed for the metadata bytes");
      errLeds();
      return false;
    }
    
    //grab id and sensor type from dataToBeSentToSerial,which are both intergers
    memcpy(dataSent,dataToBeSentToSerial,8);

    //send the data t
    bytesSend += Serial.write( dataSent, 8);  // Send id and sensor type

    free(dataSent);
 

    dataSent =(uint8_t*) malloc(sizeOfPayload);//allocate 8 bytes


    if (dataSent == NULL){
      Serial.println("Error: malloc failed for payload bytes");
      errLeds();
      return false;
    }
    //grab the rest of the data from dataToBeSentToSerial
    memcpy(dataSent,(uint8_t*)(dataToBeSentToSerial + 8),sizeOfPayload);

    bytesSend += Serial.write(dataSent, sizeOfPayload);  // Send struct data
    free(dataSent);
    free(dataToBeSentToSerial);

    if (bytesSend>8) { //metadata and payload send
        return true;
    }else{
        return false; //either no data was sent or only metadata
    } 
  }
  
void printMAC(const uint8_t MAC_ADDRESS[6]){
 Serial.printf("Device MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  MAC_ADDRESS[0], MAC_ADDRESS[1], MAC_ADDRESS[2], 
                  MAC_ADDRESS[3], MAC_ADDRESS[4], MAC_ADDRESS[5]);
}

bool checkForInactivityOverThreshold(const unsigned long timeLastMessageWasSend,unsigned long threshold){
    unsigned long current_time = millis();
    unsigned long remaining_time = current_time - timeLastMessageWasSend ;
    if (remaining_time > threshold){
        return true;
    }
    else {
        return false;
    }
}

//keep a minimum time in order to send a new message
bool isTimeToSendMessage(Setting settingOfSensor,const unsigned long timeLastMessageWasSend){
  unsigned long currentTime = millis();
  unsigned long minimumTimeToPass;
  switch (settingOfSensor){
      case(NO_WAIT):
        minimumTimeToPass = frequencyMinimum;
        break;
      case(WAIT_10_SECONDS):
        minimumTimeToPass = frequencyWhenFailureOccurs;
        break;
      default:   
        Serial.println("Some error occured");
        errLeds();
  }
  
  if (abs((double)(currentTime - timeLastMessageWasSend)) > minimumTimeToPass){
        //time to send the message
        
        return true;
  }
  else{
        return false;
 }
}

//Set the flag enum that the sensor has. It could change in some flags the time of the last message that was sent

void setTimerAndFlag(Setting setting){
  settingOfSensor = setting;
  if (settingOfSensor == NO_WAIT){
    timeLastMessageWasSend = millis();
  }
}

ResponseMessageFromReceiver createResponseFromReceiver(const int sensorMessage,bool writtenIntoQueue,bool writtenIntoPython){

  ResponseMessageFromReceiver response;
  response.id = id;
  response.writtenIntoQueue = writtenIntoQueue;
  return response;
}

String enum_recognized_Sensor_to_Strings(recognized_Sensor sensor){
  String response;
  switch (sensor){
    case NO_KNOWN_SENSOR:
        response = "NO_KNOWN_SENSOR";
        break;
    case BME680:
        response = "BME680";
        break;

    case CSS811:
       response = "CSS811";
       break;
    default:
      response = "INVALID POSITION";
  }
  return response;
}



/* Halt in case of failure */
void errLeds(void)
{
  for (;;){
    flashLeds();
  }
}
/*For debugging reasons*/

void flashLeds(){
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}