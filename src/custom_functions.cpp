
#include "custom_headers.h"


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


void connectToWifiAndServer(){
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Connecting to Wi-Fi");
  bool wifi_found = false;
  bool server_found =false;
  //continue until you find a connection
  while (wifi_found != true and server_found != true){

    for (int i = 0 ; i < numberOfWifiRouters and (wifi_found != true and server_found != true);i++){
        server_found =false;
        wifi_found = false;
        Serial.println("Trying to connect to Wi-Fi: " + connectionInformation[i].ssid);
        WiFi.begin(connectionInformation[i].ssid.c_str(), connectionInformation[i].password.c_str()); 
        delay(1000);
        if (WiFi.status() == WL_CONNECTED){
          selectedWIFI = connectionInformation[i];
          bool wifi_found = true;
          Serial.println("Wifi choosen:"+selectedWIFI.ssid);

          for (int j=0 ; j <numberOfPotentialServers and server_found != true;j++){
              
              createTheUrl(selectedWIFI.serverIp[i]);
              Serial.println("Trying to connect to server: " + selectedWIFI.serverIp[i]);
              http.begin(serverUrl);
              http.setTimeout(3000); // Set timeout to 5 seconds
              Serial.println("URL: " + serverUrl);
              int httpCode = http.GET(); 
              //code received
              if (httpCode > 0){
                  selectedIP = selectedWIFI.serverIp[i];
                  Serial.println("Connected to server at IP: " + selectedIP);
                  server_found =true;
              }

          }
        }

    }  
}
  digitalWrite(LED_BUILTIN, LOW);

}

void createTheUrl(String IP){

   serverUrl = "http://"+IP + port + endpoint;

}