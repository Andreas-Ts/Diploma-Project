#include "custom_headers.h"

bool setNetworkConnections() {
  bool haveDoneNetworkSetAction = false;
  static bool tryingToConnectToWifi = false;
  static unsigned int wifiIndex = 0;
  static int httpIndex = -1;
  static unsigned long timeFiredWifiBegin = 0;

    if (WiFi.status() != WL_CONNECTED and tryingToConnectToWifi == false ) {
        digitalWrite(LED_BUILTIN, HIGH);

        Serial.println("Wi-Fi disconnected. Reconnect to the server");
        
        Serial.println("Connect to the wifi:"+connectionInformation[wifiIndex].ssid);
        WiFi.begin(connectionInformation[wifiIndex].ssid,connectionInformation[wifiIndex].password);
        httpIndex = -1;
        timeFiredWifiBegin = millis();
        tryingToConnectToWifi = true;
        haveDoneNetworkSetAction =  true;

      }

 
  
  //if we tried to connect and it wasn't successful
  if (WiFi.status() != WL_CONNECTED and tryingToConnectToWifi == true and  
      seeTimeElapsed(timeFiredWifiBegin) > timeToConnectToWifi )
  {
      Serial.print("I didn't managed to connect to the wifi:");
      Serial.println(connectionInformation[wifiIndex].ssid);
      tryingToConnectToWifi = false;

      wifiIndex = (wifiIndex + 1) % numberOfWifiRouters; 
      haveDoneNetworkSetAction =  true;

  }

   if (WiFi.status() == WL_CONNECTED and tryingToConnectToWifi == true){
      tryingToConnectToWifi = false;
      selectedWIFI = connectionInformation[wifiIndex];
      Serial.print("I connected to the wifi:");
      Serial.println(selectedWIFI.ssid);
      haveDoneNetworkSetAction = true;

    }


  if (WiFi.status() == WL_CONNECTED and haveDoneNetworkSetAction == false and selectedIP == ""){
            digitalWrite(LED_BUILTIN, HIGH);

    httpIndex++;
    if (httpIndex>=numberOfPotentialServers){
        Serial.println("No more servers to try. I will stop trying to connect to this wifi.");
        WiFi.disconnect();
        
        httpIndex = -1; //reset the index of the http servers
        wifiIndex = (wifiIndex + 1) % numberOfWifiRouters; 
        Serial.println("wifiIndex"+String(wifiIndex));
        haveDoneNetworkSetAction =  true;

    }
    else{
      selectedIP = selectedWIFI.serverIp[httpIndex]; 
      String url = endpoint +"checkConnection";
     
      if (!connectToTCP(selectedIP)){
           Serial.println("Server not found");
           selectedIP ="";
      }
      Serial.println("I will send  to the server" + serverUrl);
      
      http.begin();
      http.setTimeout(1000); 
      http.setReuse(true); // Enables keep-alive
      int httpCode = http.GET(); 
      //code received
      if (httpCode > 0){
          selectedIP = selectedWIFI.serverIp[httpIndex];
          Serial.println("Connected to server at IP: " + selectedIP);
          statusConnectedToServer = true;
          digitalWrite(LED_BUILTIN, LOW);

        }else{
         
        }

       haveDoneNetworkSetAction =  true;

      }

  } 

return haveDoneNetworkSetAction ;

}

bool connectToTCP(String selectedIP){
 int is_connected = wifiClient.connect(selectedIP.c_str(),port.toInt(),500);
 if (wifiClient.connected()){
   return true;
 }
 else{
  return false;
 }
}