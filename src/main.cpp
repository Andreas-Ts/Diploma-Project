#include "custom_headers.h"

void setup() {
   Serial.begin(115200);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);
  readMacAddress();
  setReceiverMAC(MAC_LIBRARY,idOfTheReceiver);
  int potential_id = chooseIDBasedOfMAC(MAC_LIBRARY);
  if (potential_id<0){
    Serial.println("Problem with reading the MCA address.");
    errLeds();
  }
  else{
    id =  potential_id;
    message.id = id;
  }

 
  // Setup Sensor with enum type based of the sensor we have.You can put multiple type
 if (setupBME680()==true){
    sensorLocatedIntoDevice = BME680;
 }

 if (setupCCS811()==true){ 
    sensorLocatedIntoDevice = CSS811;
 }
   
 //If we still don't have a sensor we recognized,stop the loop
 if (sensorLocatedIntoDevice == NO_KNOWN_SENSOR){
 
      Serial.println("No known sensor was found");
      errLeds();
 } 
 //output = "My sensor is the "+ enum_recognized_Sensor_to_Strings(sensorLocatedIntoDevice);
 //Serial.println(output);
 //Set the variables into the Sensor Message and what variable at union we will use
 message.id = id;
 message.sensor = sensorLocatedIntoDevice;
 
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    errLeds();
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
  //wait 10 seconds to wait to open the python script
  delay(10 *1000);
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




