#include "custom_headers.h"

bool setNetworkConnections(bool newReadingJustOccured){
  static bool tryingConnectingWifi = false;
  static bool tryingConnectingServer = false;
  static unsigned int wifiIndex = 0;
  static int httpIndex = -1;
  static unsigned long timeFiredWifiBegin = 0;
  if (!newReadingJustOccured)  //don't do anything until we have a fresh reading
  {
      return false;
  }
  //if we are 
  if (WiFi.status() != WL_CONNECTED and tryingConnectingWifi == false ) {

    Serial.println("Wi-Fi disconnected. Reconnect to the server");
    
    wifiIndex = 0;
    Serial.println("Connect to the wifi:"+connectionInformation[wifiIndex].ssid);
    WiFi.begin(connectionInformation[wifiIndex].ssid,connectionInformation[wifiIndex].password);
    httpIndex = -1;
    timeFiredWifiBegin = millis();
    tryingConnectingWifi = true;
    tryingConnectingServer = false;
    return true;
  }
  //if we tried to connect and it wasn't successful
  if (WiFi.status() != WL_CONNECTED and tryingConnectingWifi == true and  seeTimeElapsed(timeFiredWifiBegin) > 6000 ){
      Serial.print("I didn't managed to connect to the wifi:");
      Serial.println(connectionInformation[wifiIndex].ssid);
      wifiIndex = (wifiIndex + 1) % numberOfWifiRouters; 
      return true;

  }

   if (WiFi.status() == WL_CONNECTED and tryingConnectingWifi == true){
      tryingConnectingWifi = false;
      selectedWIFI = connectionInformation[wifiIndex];
      Serial.print("I connected to the wifi:");
      Serial.println(selectedWIFI.ssid);
      tryingConnectingServer = true;
      return true;

    }


  if (WiFi.status() == WL_CONNECTED and tryingConnectingWifi == false and tryingConnectingServer == true){
    httpIndex++;
    if (httpIndex>=numberOfPotentialServers){
        tryingConnectingServer = false;
    }
    else{
    String url = createTheUrl(endpoint+"checkConnection");
    Serial.println("I will send  to the server" + selectedWIFI.serverIp[httpIndex]);

    http.begin(url);
    http.setTimeout(1500); 
    http.setReuse(true); // Enables keep-alive
    int httpCode = http.GET(); 
    //code received
    if (httpCode > 0){
        selectedIP = selectedWIFI.serverIp[httpIndex];
        Serial.println("Connected to server at IP: " + selectedIP);
        statusConnectedToServer = true;
    }else{
      Serial.println("Server not found");
    }
    }
    return true;

  } 




}
