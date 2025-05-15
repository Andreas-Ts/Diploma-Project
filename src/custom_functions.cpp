
#include "custom_headers.h"
#include "customFunctions.h"

//Initialize the values of the json object to zero
void initializemessageJSON(){
  messageJSON["DateTime"] = nullptr;
  messageJSON["Id"] = id;
  messageJSON["Sensor"] = sensorLocatedIntoDevice;
  messageJSON["BME680:iaq"] = nullptr;
  messageJSON["BME680:iaqAccuracy"] = nullptr;
  
  messageJSON["BME680:staticIaq"] = nullptr;
  messageJSON["BME680:co2Equivalent"] = nullptr;
  messageJSON["BME680:co2EquivalentAccuracy"] = nullptr;

  messageJSON["BME680:breathVocEquivalent"] = nullptr;
  messageJSON["BME680:breathVocEquivalentAccuracy"] = nullptr;

  messageJSON["BME680:rawTemperature"] = nullptr;
  messageJSON["BME680:pressure"] = nullptr;
  messageJSON["BME680:rawHumidity"] = nullptr;
  messageJSON["BME680:gasResistance"] = nullptr;
  messageJSON["BME680:stabStatus"] = nullptr;
  messageJSON["BME680:runInStatus"] = nullptr;
  messageJSON["BME680:temperature"] = nullptr;
  messageJSON["BME680:humidity"] = nullptr;
  messageJSON["BME680:gasPercentage"] = nullptr;
  messageJSON["BME680:gasPercentageAccuracy"] = nullptr;

  messageJSON["CCS811:eCO2"] = nullptr;
  messageJSON["CCS811:TVOC"] = nullptr;
  messageJSON["CCS811:RawResistance"]=nullptr;
  messageJSON["CCS811:30minutesPassed"] = minutes30HavePassed;

}

//choose id based on the MAC address of the device
//and return the id of the device
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