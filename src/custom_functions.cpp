

#include "constants.h"
#include "customFunctions.h"

bool readMacAddress(){
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, my_MAC);
  if (ret == ESP_OK) {
     printMAC(my_MAC);
     return true;
  } else {
    Serial.println("Failed to read MAC address");
    return false;
  }
}

void setReceiverMAC(const uint8_t *MAC_LIBRARY[],const int library_position){
  
  memcpy(ESP32_MAC_OF_RECEIVER, MAC_LIBRARY[library_position], 6);
}

bool setBME680(Adafruit_BME680 *bme){
  Serial.println("Hi from BME680");
  
  if (!bme->begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    return false;
  }

  Serial.print("The setTemperatureOversampling has executed ");
  Serial.println(bme->setTemperatureOversampling(BME680_OS_8X));

  Serial.print("The setTemperatureOversampling has executed ");
  Serial.println( bme->setHumidityOversampling(BME680_OS_2X));
  Serial.print("The setTemperatureOversampling has executed ");
  Serial.println(  bme->setPressureOversampling(BME680_OS_4X));
  Serial.print("The setTemperatureOversampling has executed ");
  Serial.println(bme->setIIRFilterSize(BME680_FILTER_SIZE_3));
 
  Serial.print("The setTemperatureOversampling has executed ");
  Serial.println(bme->setGasHeater(320, 150)); // 320*C for 150 ms
  ; 
  Serial.println("Bye from BME680");
  return true;
}

bool performBME680Reading(Adafruit_BME680 *bme,SensorMessage *sensorMessage,int id){


  bool success = bme->performReading();
  if (!success) {
    Serial.println(F("Failed to begin reading :("));
    return false;
  }
  /*
  Serial.print(F("Reading started at "));
  Serial.print(millis());
  Serial.print(F(" and will finish at "));
  Serial.println(endTime);
  */
   if (! bme->endReading()) {
    Serial.println(F("Failed to complete reading :("));
    return false;
  }
  sensorMessage->id = id;
  /*
  Serial.print("id of the device = ");
  Serial.print(sensorMessage->id);
  */
  sensorMessage->temperature=bme->temperature ;
  /*
  Serial.print("Temperature = ");
  Serial.print(sensorMessage->temperature);
  Serial.println(" *C");
  */
  sensorMessage->pressure=bme->pressure / 100.0;
 /*
  Serial.print("Pressure = ");
  Serial.print(sensorMessage->pressure);
  Serial.println(" hPa");
*/
  sensorMessage->humidity=bme->humidity ;
  /*
  Serial.print("Humidity = ");
  Serial.print( sensorMessage->humidity);
  Serial.println(" %");
 */ 
  sensorMessage->gas_resistance  = bme->gas_resistance / 1000.0 ;
 /* Serial.print("Gas = ");
  Serial.print( sensorMessage->gas_resistance);
  Serial.println(" KOhms");
  */
  sensorMessage->altitude=bme->readAltitude(SEALEVELPRESSURE_HPA) ;
  /*
  Serial.print("Approx. Altitude = ");
  Serial.print( sensorMessage->altitude);
  Serial.println(" m");
 */
  return true;
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


uint8_t* structToBytes(SensorMessage data) {

  uint8_t *buffer;      // Create a byte buffer
  memcpy(buffer, &data, sizeof(data));  // Copy struct to byte array
  return buffer;
}

void sendDataToSerial(const uint8_t *dataToBeSentToSerial){
 
    int bytesSend = 0;
    uint16_t struct_size = sizeof(SensorMessage);

    Serial.write( (uint8_t*) &struct_size, sizeof(struct_size));  // Send struct size (2 bytes)
    Serial.write(dataToBeSentToSerial, struct_size);  // Send struct data

  }
  
void printMAC(const uint8_t MAC_ADDRESS[6]){
 Serial.printf("Device MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  MAC_ADDRESS[0], MAC_ADDRESS[1], MAC_ADDRESS[2], 
                  MAC_ADDRESS[3], MAC_ADDRESS[4], MAC_ADDRESS[5]);
}

bool checkForInactivityOverThreshold(unsigned long *timeLastMessageWasSend,unsigned long threshold){
    unsigned long current_time = millis();
    unsigned long remaining_time = current_time - *timeLastMessageWasSend ;
    if (remaining_time > threshold){
        return true;
    }
    else {
        return false;
    }
}
void setTimerAndFlag(Setting flag,bool *waitingResponse,unsigned long *timeLastMessageWasSend){
  if (flag == WAITING){
    *timeLastMessageWasSend = millis();
    *waitingResponse = true;
  }
  if (flag == FINISHED_SUCCESSFULLY){
    *timeLastMessageWasSend = millis();
    *waitingResponse = false;
  }
  if (flag == FINISHED_STANDBY){
    //do nothing
  }
}



ResponseMessageFromReceiver createResponseFromReceiver(const int sensorMessage,bool writtenIntoQueue,bool writtenIntoPython){

  ResponseMessageFromReceiver response;
  response.id = id;
  response.writtenIntoQueue = writtenIntoQueue;
  response.writtenSuccessfully = writtenIntoPython;
  return response;
}
/*

ErrorMessage createErrorMessage(char* errorText){
  ErrorMessage error;
  error.id = id;
  strncpy(error.error,errorText,sizeof(errorText))
  return error;
}

*/