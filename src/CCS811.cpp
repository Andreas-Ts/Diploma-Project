#include "custom_headers.h"
  
  bool setupCCS811(){
    
    if(!ccs.begin()){
      Serial.println("Failed to start sensor! Please check your wiring.");
      return false;
    }
    //Serial.println("CCS811 test");
   
    ccs.setDriveMode(CCS811_DRIVE_MODE_10SEC);

    // Wait for the sensor to be ready
    while(!ccs.available());
    // Set the memory to be used for the baseline resistance
    EEPROM.begin(CCS811_EEPROM_SIZE + 1);
    loadCCS811Baseline();
    return true;
  }


  bool loopCCS811(){
    bool haveNewData = false; 
    //read data afterat interupt
  
    if (ccs.available()) {
   
      if (! ccs.readData()) {

        messageJSON["CCS811:eCO2"] = ccs.geteCO2();
        messageJSON["CCS811:TVOC"] = ccs.getTVOC();
        messageJSON["CCS811:RawResistance"]=ccs.getRawADCreading();
        haveNewData = true;
        checkIf30MinutesHavePassedCSS811(); //we calculate if the 30 minutes after activation have passed

      }
      else{
          uint8_t error = ccs.read8(CCS811_ERROR_ID);
          Serial.println("Error reading CCS811 data");
          Serial.println("error code:"+error);
          sendErrorMessage("Error reading CCS811 data",error);
      }
    }
   

    if (minutes30HavePassed == 1){
          updateCCS811Baseline();
        }
    
    //ask for url every hour
    unsigned long time_until_next_env_get = abs((int)(millis()-ENVIRONMENTAL_DATA_CCS811_TIMER));
   
    if ((firstTimeAskingEnvironmentalData == true) or (time_until_next_env_get>ENVIRONMENTAL_DATA_CCS811_FREQUENCY)){
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Asking for environmental data");
        getEnvironmentalData();
      }
    }
   
   return haveNewData;
  }

   //check if 30 minutes have passed since the activation of the device in order to activate the flag for later data analysis
  //we check if it was previous activated or not to take care of overflows
void checkIf30MinutesHavePassedCSS811(){
 
  if (minutes30HavePassed == 0 and millis()> 1000 * 60 * 30){
    minutes30HavePassed = 1;
}
}

//if 30 minutes have passed since the activation of the device get the baseline resistance
void loadCCS811Baseline(){
if (EEPROM.read(0) == CCS811_EEPROM_SIZE) {
    // Existing state in EEPROM
    Serial.println("Reading baseline Resistance from EEPROM");

   // Read two bytes from EEPROM
  byte lowByte = EEPROM.read(1);   // address 1
  byte highByte = EEPROM.read(2);  // address 2
 
  // Combine them into a 16-bit integer that is the baseline resistance
  uint16_t number = ((uint16_t)highByte << 8) | lowByte;
  Serial.println(String(number));
  ccs.setBaseline(number);
  } else {
    // Erase the EEPROM with zeroes
    Serial.println("Erasing EEPROM");
    EEPROM.write(0,CCS811_EEPROM_SIZE);  
    for (uint8_t i = 0 + 1; i < CCS811_EEPROM_SIZE ; i++)
    EEPROM.write(i, 0);

    EEPROM.commit();
  }
}
void updateCCS811Baseline(){
  bool update = false;
  if (stateUpdateCounter == 0) {
    /* First state update when IAQ accuracy is >= 3 */
    if (minutes30HavePassed==1) {
      update = true;
      stateUpdateCounter++;
    }
  } else {
    /* Update every STATE_SAVE_PERIOD minutes */
    if ((stateUpdateCounter * STATE_SAVE_PERIOD) < millis()) {
      update = true;
      stateUpdateCounter++;
    }
  }

  if (update) {
    uint16_t baseline = ccs.getBaseline();
    Serial.println("Writing baseline Resistance to EEPROM");


    for (uint8_t i = 0; i < CCS811_EEPROM_SIZE ; i++) {
      EEPROM.write(i + 1, bsecState[i]);
      Serial.println(bsecState[i], HEX);
    }

    EEPROM.commit();
  }
}