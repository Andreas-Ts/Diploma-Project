
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
  Serial.println("so:"+String(res));
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
  while (wifi_found != true || server_found != true){
    wifi_found = false;

    for (int i = 0 ; ((i < numberOfWifiRouters) || (wifi_found != true && server_found != true));i++){
       
        server_found =false;
        Serial.println("Trying to connect to Wi-Fi: " + String(connectionInformation[i].ssid) + " .With password: "+connectionInformation[i].password);
        WiFi.begin(connectionInformation[i].ssid.c_str(), connectionInformation[i].password.c_str()); 
        Serial.println(connectionInformation[i].ssid.c_str());
          Serial.println(connectionInformation[i].password.c_str());
        Serial.println("Before delay" + String(WiFi.status()));
        unsigned long startAttemptTime = millis();
        const unsigned long wifiTimeout = 10000; // 10 seconds
          while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < wifiTimeout) {
            delay(500);
            Serial.print(".");
          }
        Serial.println();
        Serial.println("After delay"+String(WiFi.status()));
        if (WiFi.status() == WL_CONNECTED){
          selectedWIFI = connectionInformation[i];
          wifi_found = true;
          Serial.println("Wifi choosen:"+selectedWIFI.ssid);

          for (int j=0 ; j <numberOfPotentialServers and server_found != true;j++){
              HTTPClient http;
              selectedIP = selectedWIFI.serverIp[j];
              createTheUrl(endpoint+"checkConnection");
              Serial.println("Trying to connect to server: " + selectedWIFI.serverIp[j]);
              http.begin(serverUrl);
              http.setTimeout(2000); // Set timeout to 5 seconds
              Serial.println("URL: " + serverUrl);
              int httpCode = http.GET(); 
              //code received
              if (httpCode > 0){
                  selectedIP = selectedWIFI.serverIp[j];
                  Serial.println("Connected to server at IP: " + selectedIP);
                  server_found =true;
              }else{
                Serial.println("Server not found");
              }
              http.end();

          }
        }
        else{
          WiFi.disconnect(true);  // Disconnect and erase old config
          delay(1000);
        }

    }  
}
  digitalWrite(LED_BUILTIN, LOW);

}

void createTheUrl(String endpoint){

   serverUrl = "http://"+ selectedIP + port + endpoint;

}

void scanWiFiNetworks() {
  Serial.println("Scanning for WiFi networks...");

  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("No networks found.");
  } else {
    Serial.printf("%d networks found:\n", n);
    for (int i = 0; i < n; ++i) {
      Serial.printf("%d: %s (%d dBm) %s\n", 
                    i + 1,
                    WiFi.SSID(i).c_str(),
                    WiFi.RSSI(i),
                    (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Open" : "Secured");
      delay(10);
    }
  }
  Serial.println();
}

 
  ///This function is used to loop the sensor and get the data from it. It will be called in the main loop of the code.
  ///It will check if the sensor is BME680 or CCS811 and call the appropriate function to get the data from it.
  ///It will also check if the data is new and if it is, it will tell the loop to send message into the wifi
  bool loopSensor(){
    bool haveNewData = false;

      if ( sensorLocatedIntoDevice== "BME680"){
        haveNewData = loopBME680();

      }
      if (sensorLocatedIntoDevice== "CCS811"){
        haveNewData = loopCCS811();
      }
    return haveNewData;
  }

  //Make a http get request to the server to get the environmental data of the room
void getEnvironmentalData(){
    HTTPClient http;
    Serial.println("Asking data from the server...");
    //default url + the specified endpoint
    createTheUrl(endpoint +"getEnvRoomData");
    Serial.println(serverUrl);
    http.begin(serverUrl);
    http.setTimeout(2000); // Set timeout to 2 seconds
    int httpResponseCode  = http.GET();
     if (httpResponseCode == HTTP_CODE_OK) {

        String stringResponse = http.getString();
        Serial.println(stringResponse);
        JsonDocument response;
        deserializeJson(response,stringResponse);
        Serial.println("Response:");
        temperature = response["temperature"];
        humidity = response["humidity"];
        Serial.print("Room Temperature: ");
        Serial.println(temperature);
        Serial.print("Room Humidity: ");  
        Serial.println(humidity);
        ccs.setEnvironmentalData(humidity, temperature);
        

    } else {
        Serial.print("POST failed. Error: ");
        Serial.println(http.errorToString(httpResponseCode).c_str());  // Get error description
           
    }
    firstTimeAskingEnvironmentalData = false; //we have asked the environmental data for the first time
    ENVIRONMENTAL_DATA_CCS811_TIMER = millis(); //set timer 
   // End HTTP connection
    http.end();
}


void sendErrorMessage(String error_message,uint8_t error){
  flashLeds();
 HTTPClient http;
  JsonDocument jsonError;
    Serial.println("Sending data to server...");
    createTheUrl(endpoint +"postError");

    Serial.println("The url is:"+(serverUrl));
    http.begin((serverUrl));//in case of adding more url sections into the url we send the post request

    http.addHeader("Content-Type", "application/json");
    http.setTimeout(2000);
    jsonError["error_number"] = error;
    jsonError["error_message"] = error_message;
    // Serialize the  JSON
    String messageJSONString;
    serializeJsonPretty(jsonError, messageJSONString);
    Serial.println(messageJSONString);
    // Send POST request
    int httpResponseCode = http.POST(messageJSONString);
    Serial.println("The wifi is being:"+ String(WL_CONNECTED));
    // Print response
    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("Response:");
        Serial.println(response);
    } else {
        Serial.print("POST failed to arrive at server. Error: ");
        Serial.println(http.errorToString(httpResponseCode).c_str());  // Get error description
        
    }

    http.end();

}

void connecToWifi