
#include "custom_headers.h"
#include "customFunctions.h"




//choose id based on the MAC address of the device
//and return the id of the device
void chooseIDBasedOfMAC(){
  
  esp_err_t  res= esp_wifi_get_mac(WIFI_IF_STA,my_MAC);
  Serial.println("so:"+String(res));
  if (res != ESP_OK){
     Serial.println("Failed to read MAC address");
     errLeds();
  }
  Serial.println(ESP32_TOTAL_DEVICES_NUMBER);
  for ( int i =0;  i < ESP32_TOTAL_DEVICES_NUMBER; i++ ){
    if (! memcmp(MAC_LIBRARY[i],my_MAC,sizeof(my_MAC))){
       Serial.println("My id is"+ String(id));
       id = i;
    }
  }
  //only positive values are legit
  if (id < 0 ){
       Serial.println("Failed to find a saved MAC address");
      errLeds();

  }
  
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




String createTheUrl(String endpoint){

   serverUrl = "http://"+ selectedIP + port + endpoint;
    return serverUrl;
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

void setupSensor(){
  // Setup Sensor with enum type based of the sensor we have.You can put multiple type
 if (setupBME680()==true){
    Serial.println("BME680 sensor found");
    sensorLocatedIntoDevice = "BME680";
 }

 if (setupCCS811()==true){ 
  Serial.println("CCS811 sensor found");
    sensorLocatedIntoDevice = "CCS811";
 }
   
 //If we still don't have a sensor we recognized,stop the loop
 if (sensorLocatedIntoDevice=="NO_KNOWN_SENSOR"){
 
      Serial.println("No known sensor was found");
      errLeds();
 } 
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


void wifiEventConnectedToWifiWithIP(WiFiEvent_t event, WiFiEventInfo_t info){
    timerStop(wifiTimer);
    Serial.println("ESP32 connected to Wifi with ssid:"+connectionInformation[wifiIndex].ssid);    
    //asign the wifi found as the selected wifi
    selectedWIFI = connectionInformation[wifiIndex];
    Serial.println("Time to connect to server");
    statusConnectedToWifi = true;    
              
}
void resetConnectionStatus(){
    bool statusConnectedToWifi = false;
    bool statusConnectedToServer = false;
    unsigned int wifiIndex = 0;
    unsigned int httpIndex = 0;
}


void connectToServerHandler(){
    if (httpIndex >= numberOfPotentialServers){
      Serial.println("The wifi"+connectionInformation[wifiIndex].ssid+"doesn\'t have any server any eligible server running.");
      wifiNoConnection();
      return;
    }

}


//wifi was not connected in the given time or handler didn't found any server
void wifiNoConnection(){
    Serial.println("The wifi "+String(connectionInformation[wifiIndex].ssid)+" didn't connect.");
    //go to the next one,either 1 from 0 or 0 from 1
    statusConnectedToWifi = false;
}

unsigned int seeTimeElapsed(unsigned long pastTime){
    unsigned long currentTime = millis();
    unsigned int timeElapsed = (int) currentTime - pastTime;
    if (timeElapsed < 0){
      Serial.println("Past time given is in future.Something wrong happened");
      flashLeds();
    } 
    return timeElapsed;
}
String getLocalDateTime(){
   // Begin HTTP connection
  struct tm timeinfo;

  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    errLeds();
  }else{
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  }
  
  char isoTime[25];  // Buffer for ISO string
  strftime(isoTime, sizeof(isoTime), "%Y-%m-%dT%H:%M:%S", &timeinfo);
   // Append fixed timezone
  String finalTime = String(isoTime) + "+00:00";
  return finalTime;
}


void ramAvailable(){
  Serial.println("Free heap: " + String(ESP.getFreeHeap()) + " bytes");
  Serial.println("Total heap: " + String(ESP.getHeapSize()) + " bytes");
  Serial.println("Minimum free heap: " + String(ESP.getMinFreeHeap()) + " bytes");
  Serial.println("Max allocatable heap: " + String(ESP.getMaxAllocHeap()) + " bytes");
  Serial.println("Free PSRAM: " + String(ESP.getFreePsram()) + " bytes");
  Serial.println("Sketch size: " + String(ESP.getSketchSize()) + " bytes");
  Serial.println("Sketch free space: " + String(ESP.getFreeSketchSpace()) + " bytes");
}