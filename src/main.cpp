#include <constants.h>
#include <customFunctions.h>

void setup() {
   Serial.begin(115200);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  delay(50);
   readMacAddress();
  setReceiverMAC(MAC_LIBRARY,1);
  
  int potential_id = chooseIDBasedOfMAC(MAC_LIBRARY);
  if (potential_id<0){
    Serial.println("Problem with reading the MCA address.");
    is_Everything_Ok= false;
  }
  else{
    Serial.print("The id of device choosen:");
    Serial.println(potential_id);

    id =  potential_id;
    sensorMessage.id = id;
  }


   // Setup Sensor
  is_Everything_Ok = setBME680(&bme);
  
  
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    //Serial.println("Error initializing ESP-NOW");
    is_Everything_Ok= false;
  }
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
 
  delay(10);


  if (isTheReceiverESP32NOW(my_MAC)){
      setupReceiver();
  }
  else{
      setupSender();
  }
  
}

void loop() {
  if (is_Everything_Ok == false){
     Serial.println("An error occurred into the program. Shutdown the device");
  }

  do {} while (is_Everything_Ok == false);
  
  if (isTheReceiverESP32NOW(my_MAC)){
   loopReceiver();
  }
  else{
   loopSender();
   }
}




