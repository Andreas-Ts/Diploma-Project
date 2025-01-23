#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include "constants.h"
#include "customFunctions.h"
#include <WiFi.h>
#include <esp_wifi.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define SEALEVELPRESSURE_HPA (1013.25)


// Set up oversampling and filter initialization
void setReceiverMAC(const uint8_t targetMAC[6]){
  
  memcpy(ESP32_MAC_OF_RECEIVER, targetMAC, sizeof(6));
}



void setBME680(Adafruit_BME680 *bme){

  if (!bme->begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }

   
  bme->setTemperatureOversampling(BME680_OS_8X);
  bme->setHumidityOversampling(BME680_OS_2X);
  bme->setPressureOversampling(BME680_OS_4X);
  bme->setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme->setGasHeater(320, 150); // 320*C for 150 ms
  
}

void performBME680Reading(Adafruit_BME680 *bme,SensorMessage *myData){


if (!bme->performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
  myData->id = id;
  Serial.print("id of the device = ");
  Serial.print(myData->id);
  Serial.println(" *C");

  myData->temperature=bme->temperature ;
  Serial.print("Temperature = ");
  Serial.print(myData->temperature);
  Serial.println(" *C");
  
  myData->pressure=bme->pressure / 100.0;
  Serial.print("Pressure = ");
  Serial.print(myData->pressure);
  Serial.println(" hPa");

  myData->humidity=bme->humidity ;
  Serial.print("Humidity = ");
  Serial.print( myData->humidity);
  Serial.println(" %");
  
  myData->gas_resistance  = bme->gas_resistance / 1000.0 ;
  Serial.print("Gas = ");
  Serial.print( myData->gas_resistance);
  Serial.println(" KOhms");
  
  myData->altitude=bme->readAltitude(SEALEVELPRESSURE_HPA) ;
  Serial.print("Approx. Altitude = ");
  Serial.print( myData->altitude);
  Serial.println(" m");

  Serial.print("Came from device with MAC address:")
  Serial.println(makeMACAdressEasilyPrintable());
  Serial.println();
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
}

uint8_t amITheReceiverESP32NOW(){
  if (! memcmp(ESP32_MAC_OF_RECEIVER,my_MAC,sizeof(my_MAC))) {
    return 1;
  }  
  else {
    return 0;
  }
}

char* makeMACAdressEasilyPrintable(uint8_t mac_addr[6]){
  char macAdressStr[18];
  snprintf(macAdressStr, sizeof(macAdressStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  return macAdressStr;
}