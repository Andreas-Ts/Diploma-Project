#include "custom_headers.h"



void setup() {
  
  Serial.begin(115200);
  delay(1000);
  Serial.println("hi");
  scanWiFiNetworks();
  setupConnectionInformation(); 
    connectToWifiAndServer();
 
  // Connect to Wi-Fi


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

}

void loop() {


  bool hasNewData =loopSensor();
  if (millis()- timeSinceLastReading>timeSinceLastReadingMaxTolerance){
    sendErrorMessage("Reading sensor has stopped",0);
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected. Reconnecting...");
    connectToWifiAndServer();
  }
  

  if (hasNewData){ //a new value has been read from the sensor 
    // Begin HTTP connection
    HTTPClient http;
    timeSinceLastReading = millis();
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




