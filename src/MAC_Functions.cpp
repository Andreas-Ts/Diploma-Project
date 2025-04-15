
#include "custom_headers.h"
#include "customFunctions.h"

bool readMacAddress(){
    esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, my_MAC);
    if (ret == ESP_OK) {
     //  printMAC(my_MAC);
       return true;
    } else {
      Serial.println("Failed to read MAC address");
      return false;
    }
  }
  
  void setReceiverMAC(const uint8_t *MAC_LIBRARY[],const int library_position){
    
    memcpy(ESP32_MAC_OF_RECEIVER, MAC_LIBRARY[library_position], 6);
  }
  
  
int chooseIDBasedOfMAC(const uint8_t *MAC_LIBRARY[]){
  int id;
  esp_err_t  res= esp_wifi_get_mac(WIFI_IF_STA,my_MAC);
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


//The memcmp returns 0 when both of the data are equal
bool isTheReceiverESP32NOW(const uint8_t *MAC_ADDRESS){
  if (memcmp(ESP32_MAC_OF_RECEIVER,MAC_ADDRESS,6) == 0) {
    return true;
  }  
  else {
    return false;
  }
}

void sendMessagesToAllOtherDevices(const uint8_t *MAC_LIBRARY[],int id,const uint8_t* response){
  for (int i =0;i < ESP32_TOTAL_DEVICES_NUMBER;i++){
    //send messages to all devices except myself
      if (i != id){
        esp_now_send(MAC_LIBRARY[i],response, sizeof(*response)); 
      }
  }
}

void printMAC(const uint8_t MAC_ADDRESS[6]){
  Serial.printf("Device MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                   MAC_ADDRESS[0], MAC_ADDRESS[1], MAC_ADDRESS[2], 
                   MAC_ADDRESS[3], MAC_ADDRESS[4], MAC_ADDRESS[5]);
 }
 