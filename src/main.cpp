#include "custom_headers.h"

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600 * 2;//3600 seconds= 1 hour, so timezone 2 hours
const int   daylightOffset_sec = 3200;

int wifiIndex = 0;
int httpIndex = 0;

void connectionWifiEvent(WiFiEvent_t event, WiFiEventInfo_t info){

}
void connectToWifi(){
  
  Wifi.begin(connectionInformation[wifiIndex].ssid,connectionInformation[wifiIndex].password);
  

}
void setup() {
  
  Serial.begin(115200);
  delay(1000);

  Wifi.onEvent(connectionWifiEvent,ARDUINO_EVENT_WIFI_STA_START);
  Serial.println("hi");
  //scanWiFiNetworks();
  setupConnectionInformation(); 


  //connectToWifiAndServer();
 
  // Connect to Wi-Fi

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  id = chooseIDBasedOfMAC(MAC_LIBRARY);

  Serial.println(String(id));

  if (id<0){
    Serial.println("Problem with reading the MCA address.");
    errLeds();
  }
 
  Serial.println("\nConnected to Wi-Fi");
  //for error handling
  pinMode(LED_BUILTIN, OUTPUT);
 

 // Setup Sensor with enum type based of the sensor we have.You can put multiple type
 if (setupBME680()==true){
    Serial.println("BME680 sensor found");
    sensorLocatedIntoDevice = "BME680";
 }

 if (setupCCS811()==true){ 
    sensorLocatedIntoDevice = "CCS811";
 }
   
 //If we still don't have a sensor we recognized,stop the loop
 if (sensorLocatedIntoDevice=="NO_KNOWN_SENSOR"){
 
      Serial.println("No known sensor was found");
      errLeds();
 } 
 Serial.println("I have the "+ sensorLocatedIntoDevice + " sensor");
 //initialize the message json
  initializemessageJSON();

 timeSinceLastReading = millis();
}

void loop() {


  bool hasNewData =loopSensor();
  if (millis()- timeSinceLastReading>timeSinceLastReadingMaxTolerance){
    sendErrorMessage("Reading sensor has stopped",0);
    timeSinceLastReading = millis();
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected. Reconnecting...");
    connectToWifiAndServer();
     timeSinceLastReading = millis();

  }
  

  if (hasNewData){ //a new value has been read from the sensor 
    // Begin HTTP connection
        struct tm timeinfo;

    if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    HTTPClient http;
    timeSinceLastReading = millis();
    char isoTime[25];  // Buffer for ISO string
    strftime(isoTime, sizeof(isoTime), "%Y-%m-%dT%H:%M:%S", &timeinfo);
    // Append fixed timezone
    String finalTime = String(isoTime) + "+00:00";
    messageJSON["DateTime"]  = isoTime;
    Serial.println("Sending data to server...");
    createTheUrl(endpoint +"postTimeSeriesData");

    Serial.println("The url is:"+(serverUrl));
    http.begin((serverUrl));//in case of adding more url sections into the url we send the post request

    http.addHeader("Content-Type", "application/json");
    http.setTimeout(2000);

    // Serialize the  JSON
    String messageJSONString;
    serializeJsonPretty(messageJSON, messageJSONString);
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

    // End HTTP connection
    http.end();
    //clear the field message 
    initializemessageJSON();
    if (httpResponseCode<0){
      Serial.println("Server disconnected. Reconnecting...");
        connectToWifiAndServer();
    }
}



}




