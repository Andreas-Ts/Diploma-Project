#include "custom_headers.h"



void setup() {
  
  Serial.begin(115200);
  delay(1000);
  setupConnectionInformation();  
  // Connect to Wi-Fi


  id = chooseIDBasedOfMAC(MAC_LIBRARY);


  connectToWifiAndServer();

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
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected. Reconnecting...");
    connectToWifiAndServer();
  }




if (loopSensor()){ //a new value has been read from the sensor 
    // Begin HTTP connection
    Serial.println("Sending data to server...");
    http.begin((serverUrl+"postTimeSeriesData"));//in case of adding more url sections into the url we send the post request
    http.addHeader("Content-Type", "application/json");
    // Serialize the  JSON
    String messageJSONString;
    serializeJsonPretty(messageJSON, messageJSONString);

    Serial.println(messageJSONString);
    // Send POST request
    int httpResponseCode = http.POST(messageJSONString);

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




