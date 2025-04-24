#include "custom_headers.h"




void setup() {

  Serial.begin(115200);
  delay(1000);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
  }
  id = chooseIDBasedOfMAC(MAC_LIBRARY);
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
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    if (loopSensor())
    { // Begin HTTP connection
      Serial.println("Sending data to server...");
      http.begin(serverUrl);
      http.addHeader("Content-Type", "application/json");
      http.setTimeout(5000); // Set timeout to 5 seconds
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
          Serial.print("POST failed. Error: ");
          Serial.println(http.errorToString(httpResponseCode).c_str());  // Get error description
        
          flashLeds();//flash to show fail at post
      }

      // End HTTP connection
      http.end();
      //clear the field message 
      initializemessageJSON();
    }
  
} else {
    Serial.println("Wi-Fi not connected");
    delay(10000); // Wait 10 seconds before checking agains

}

}




