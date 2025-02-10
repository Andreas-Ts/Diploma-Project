#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include "constants.h"
#include "customFunctions.h"

#include <WiFi.h>
#include <esp_wifi.h>


void readMacAddress(){
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, my_MAC);
  if (ret == ESP_OK) {
     printMAC(my_MAC);
  } else {
    Serial.println("Failed to read MAC address");
  }
}

void setReceiverMAC(const uint8_t targetMAC[6]){
  
  memcpy(ESP32_MAC_OF_RECEIVER, targetMAC, 6);
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

bool performBME680Reading(Adafruit_BME680 *bme,SensorMessage *myData,int id){


  unsigned long endTime = bme->beginReading();
  if (endTime == 0) {
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
  myData->id = id;
  /*
  Serial.print("id of the device = ");
  Serial.print(myData->id);
  */
  myData->temperature=bme->temperature ;
  /*
  Serial.print("Temperature = ");
  Serial.print(myData->temperature);
  Serial.println(" *C");
  */
  myData->pressure=bme->pressure / 100.0;
 /*
  Serial.print("Pressure = ");
  Serial.print(myData->pressure);
  Serial.println(" hPa");
*/
  myData->humidity=bme->humidity ;
  /*
  Serial.print("Humidity = ");
  Serial.print( myData->humidity);
  Serial.println(" %");
 */ 
  myData->gas_resistance  = bme->gas_resistance / 1000.0 ;
 /* Serial.print("Gas = ");
  Serial.print( myData->gas_resistance);
  Serial.println(" KOhms");
  */
  myData->altitude=bme->readAltitude(SEALEVELPRESSURE_HPA) ;
  /*
  Serial.print("Approx. Altitude = ");
  Serial.print( myData->altitude);
  Serial.println(" m");
 */
  return true;
}

int chooseIDBasedOfMCA(){
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
uint8_t amITheReceiverESP32NOW(){
  if (memcmp(ESP32_MAC_OF_RECEIVER,my_MAC,6) == 0) {
    return 1;
  }  
  else {
    return 0;
  }
}


uint8_t* structToBytes(SensorMessage data) {

  uint8_t *buffer;      // Create a byte buffer
  memcpy(buffer, &data, sizeof(data));  // Copy struct to byte array
  return buffer;
}

void sendDataToSerial(const uint8_t *myData){
 // Serial.println("The size of the message is:");
 // Serial.println(lengthOfSensorMessage);
   
    uint16_t struct_size = sizeof(SensorMessage);

    Serial.write( (uint8_t*) &struct_size, sizeof(struct_size));  // Send struct size (2 bytes)
    Serial.write(myData, struct_size);  // Send struct data

}
void printMAC(const uint8_t myMAC[6]){
 Serial.printf("Device MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  my_MAC[0], my_MAC[1], my_MAC[2], 
                  my_MAC[3], my_MAC[4], my_MAC[5]);
}
