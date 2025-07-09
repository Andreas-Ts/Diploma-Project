#include "custom_headers.h"

    unsigned int wifiIndex = 1;
   int httpIndex = -1;
   unsigned int passwordIndex =  1;
   unsigned long timeFiredWifiBegin = 0;
   bool tryingToConnectToWifi = false;
  bool needToScanWifi = true;
bool setNetworkConnections() {
  
  bool haveDoneNetworkSetAction = false;
   
    
    if (WiFi.status() != WL_CONNECTED and tryingToConnectToWifi == false and !haveDoneNetworkSetAction) {
        digitalWrite(LED_BUILTIN, HIGH);

        Serial.println("Wi-Fi disconnected. Reconnect to the server");
        
        Serial.println("Connect to the wifi:"+wifiNames[wifiIndex]);
        WiFi.begin(wifiNames[wifiIndex],wifiPasswords[passwordIndex]);
        httpIndex = -1;
        timeFiredWifiBegin = millis();
        tryingToConnectToWifi = true;
        haveDoneNetworkSetAction =  true;

      }

  
  //if we tried to connect and it wasn't successful
  if ((WiFi.status() != WL_CONNECTED and tryingToConnectToWifi == true and  
      seeTimeElapsed(timeFiredWifiBegin) > timeToConnectToWifi ) or WiFi.status()== WL_CONNECT_FAILED
    and !haveDoneNetworkSetAction)
  {
      Serial.print("I didn't managed to connect to the wifi:");
      Serial.println(wifiNames[wifiIndex]);
      
      incrementPasswordIndex();
      tryingToConnectToWifi = false;

      haveDoneNetworkSetAction =  true;

  }

   if (WiFi.status() == WL_CONNECTED and tryingToConnectToWifi == true and haveDoneNetworkSetAction ==  false and !haveDoneNetworkSetAction){
      tryingToConnectToWifi = false;
      selectedWIFI = wifiNames[wifiIndex];
      Serial.print("I connected to the wifi:");
      Serial.println(wifiNames[wifiIndex]);
      haveDoneNetworkSetAction = true;

    }


  if (WiFi.status() == WL_CONNECTED and haveDoneNetworkSetAction == false and 
  selectedIP == "" and !haveDoneNetworkSetAction){
    digitalWrite(LED_BUILTIN, HIGH);

   IPAddress serverIP;
    if (!WiFi.hostByName(mDNSname, serverIP)) {   
        Serial.println("The server isn't located in this wifi");
        selectedWIFI = "";
        noServerFound();
        haveDoneNetworkSetAction = true;

    }
   
    else{
        wifiClient = WiFiClient();  // Replaces existing object with a fresh one
      selectedIP = serverIP.toString(); 
      Serial.println("The IP i have"+selectedIP);
      String url = endpoint +"checkConnection";
      String serverUrltemp= createTheUrl(url);
      Serial.println("Sending data to server..."+serverUrltemp);
      Serial.println("The url is:"+(serverUrltemp));
      if (!connectToTCP(selectedIP)){
          noServerFound();
      }
      else{
      Serial.println("I will send  to the server" + url);
      Serial.println("String(wifiClient.connected():"+String(wifiClient.connected()));

      http.begin(serverUrltemp);
      
      http.setTimeout(1500); 
      http.setReuse(true); // Enables keep-alive
      int httpCode = http.GET(); 
      Serial.println("WIFI STATUS:"+String(WiFi.status()));
      Serial.println("TCP CONNECTION:"+String(wifiClient.connected()));
      Serial.println("Http code:"+String(httpCode));
      //code received
      if (httpCode > 0){
        String response = http.getString();
        Serial.println("Response:");
        Serial.println(response);
          Serial.println("Connected to server at IP: " + selectedIP);
          statusConnectedToServer = true;
          digitalWrite(LED_BUILTIN, LOW);
          serverLostConnection = false;
  
          Serial.println("String(wifiClient.connected():"+String(wifiClient.connected()));

        }
        else{
          Serial.println("GET request failed.");
          noServerFound();
        }

       haveDoneNetworkSetAction =  true;

      }

  } 


}
return haveDoneNetworkSetAction ;

}
bool connectToTCP(String selectedIP){
 int is_connected = wifiClient.connect(selectedIP.c_str(),port.toInt(),1000);
 if (is_connected){
   return true;
 }
 else{
  return false;
 }
}

void incrementWiFiIndex(){


  wifiIndex ++;
    if (wifiIndex >numberOfWifiRouters){
      wifiIndex =1;
    }
   
      Serial.println("We will try the next wifi we have available");

    
}

void incrementPasswordIndex(){
    passwordIndex++;
    if (passwordIndex > numberOfAvailablePasswords){
       incrementWiFiIndex();
        passwordIndex=1;
    }
    else {
      Serial.println("We will try the next password we have available");
          }
  }


void noServerFound(){
      Serial.println("Server not found but the mdns is correct.Perhaps connected to another device with similar local dns or server is not running");
      selectedIP ="";
      serverLostConnection = true;
    Serial.println("aaaaaa"+String(wifiClient.connected()));
      if (wifiClient.connected()){
        disconnectFromTCP();
        Serial.println("Disconnect from tcp connection ");
      }
      Serial.println("Disconnect from wifi.");
      WiFi.disconnect();
      incrementWiFiIndex();
      delay(100);

    
}

void disconnectFromTCP(){

  
   http.setReuse(false);
   http.end();
   wifiClient.stop();
  delay(100);

}
