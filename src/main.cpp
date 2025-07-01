#include "custom_headers.h"



void setup() {
  
  Serial.begin(115200);
  //See all the available WiFi networks

  //Configure all the  Wifi ssids,their paswords and the server IPs which we are going to use

  setupConnectionInformation();
   // connect to wifi and server
  while (selectedIP == ""){
    setNetworkConnections();
  } 
  //Flash led to have for connection purposes
  pinMode(LED_BUILTIN, OUTPUT);
 
 
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
   //based of MAC address, choose the id of the device
  chooseIDBasedOfMAC();

  setupSensor();

  getLocalDateTime();


}

void loop() {
  static 
  bool newReadingJustOccured =false;

  newReadingJustOccured = loopSensor();
  bool hasDoneNetworkSetAction =false;
  ////////////////////////
    if (wifiClient.connected() and newReadingJustOccured == true and serverLostConnection != false){
    hasDoneNetworkSetAction = true;
  }
  if (newReadingJustOccured and !hasDoneNetworkSetAction and (WiFi.status() != WL_CONNECTED or serverLostConnection)) {
    Serial.println("Trying to Reconnect");
    hasDoneNetworkSetAction = setNetworkConnections();
  }

  if (newReadingJustOccured and !hasDoneNetworkSetAction and !serverLostConnection){ //a new value has been read from the sensor 
    
    String url = endpoint +"postTimeSeriesData";
    String serverUrltemp= createTheUrl(url);
      Serial.println("Sending data to server..."+serverUrltemp);

    http.begin(wifiClient,serverUrltemp);//in case of adding more url sections into the url we send the post request

    http.addHeader("Content-Type", "application/json");
    http.setTimeout(1500);

    // Serialize the  JSON buffer
    String messageJSONString;
    serializeJsonPretty(buffer, messageJSONString);
    // Send POST request
    int httpResponseCode = http.POST(messageJSONString);
    // Print response
    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("Response:");
        Serial.println(response);
        clearBufferAndCounter();

    } else {
        Serial.print("POST failed to arrive at server. Error: ");
        //time to send to new server:
        disconnectFromTCP();
        Serial.println(http.errorToString(httpResponseCode).c_str());  // Get error description
        
    }


}



}




