#include "custom_headers.h"
#include "customFunctions.h"




bool setupBME680(){

    iaqSensor.begin(BME68X_I2C_ADDR_HIGH, Wire);
    output = "\nBSEC library version " + String(iaqSensor.version.major) + "," + String(iaqSensor.version.minor) + "," + String(iaqSensor.version.major_bugfix) + "," + String(iaqSensor.version.minor_bugfix);
   // Serial.println(output);
    //Perhaps the bme680 doesn't exist so we return false
    if (checkIaqSensorStatus(true) == false){
      return false;
    }
    EEPROM.begin(BSEC_MAX_STATE_BLOB_SIZE + 1);
    
    iaqSensor.setConfig(bsec_config_iaq);
    //check everything is ok with the sensor
    checkIaqSensorStatus(false);


   
    // Load the state from EEPROM
    // This is needed for the BSEC library to save the state even at the power-off
    loadBME680State();

    bsec_virtual_sensor_t sensorList[13] = {
      BSEC_OUTPUT_IAQ,
      BSEC_OUTPUT_STATIC_IAQ,
      BSEC_OUTPUT_CO2_EQUIVALENT,
      BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
      BSEC_OUTPUT_RAW_TEMPERATURE,
      BSEC_OUTPUT_RAW_PRESSURE,
      BSEC_OUTPUT_RAW_HUMIDITY,
      BSEC_OUTPUT_RAW_GAS,
      BSEC_OUTPUT_STABILIZATION_STATUS,
      BSEC_OUTPUT_RUN_IN_STATUS,
      BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
      BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
      BSEC_OUTPUT_GAS_PERCENTAGE
    };
  
    iaqSensor.updateSubscription(sensorList, 13, BSEC_SAMPLE_RATE_LP);
    checkIaqSensorStatus(false);
   
    // Print the header
    output = "Timestamp [ms], IAQ, IAQ accuracy, Static IAQ, CO2 equivalent, breath VOC equivalent, raw temp[°C], pressure [hPa], raw relative humidity [%], gas [Ohm], Stab Status, run in status, comp temp[°C], comp humidity [%], gas percentage";
    //Serial.println(output);
    return true;
  }
  
  
  
  bool setupCCS811(){
    
    if(!ccs.begin()){
      Serial.println("Failed to start sensor! Please check your wiring.");
      return false;
    }
    //Serial.println("CCS811 test");
   
    ccs.setDriveMode(CCS811_DRIVE_MODE_1SEC);

    // Wait for the sensor to be ready
    while(!ccs.available());
    // Set the memory to be used for the baseline resistance
    EEPROM.begin(CCS811_EEPROM_SIZE + 1);
    loadCCS811Baseline();
    return true;
  }
  ///This function is used to loop the sensor and get the data from it. It will be called in the main loop of the code.
  ///It will check if the sensor is BME680 or CCS811 and call the appropriate function to get the data from it.
  ///It will also check if the data is new and if it is, it will tell the loop to send message into the wifi
  bool loopSensor(){
    bool haveNewData = false;

      if ( sensorLocatedIntoDevice== "BME680"){
        haveNewData = loopBME680();

      }
      if (sensorLocatedIntoDevice== "CCS811"){
        haveNewData = loopCCS811();
      }
    return haveNewData;
  }
  
  bool loopBME680(){
    bool haveNewData = false; 

    if (iaqSensor.run()) { // If new data is available
      haveNewData = true; 


      messageJSON["BME680:iaq"] = iaqSensor.iaq;
      messageJSON["BME680:iaqAccuracy"] = iaqSensor.iaqAccuracy;
      messageJSON["BME680:staticIaq"] = iaqSensor.staticIaq;
      messageJSON["BME680:co2Equivalent"] = iaqSensor.co2Equivalent;
      messageJSON["BME680:co2EquivalentAccuracy"] = iaqSensor.co2Accuracy;
      messageJSON["BME680:breathVocEquivalent"] = iaqSensor.breathVocEquivalent;
      messageJSON["BME680:breathVocEquivalentAccuracy"] = iaqSensor.breathVocAccuracy;
      messageJSON["BME680:rawTemperature"] = iaqSensor.rawTemperature;
      messageJSON["BME680:pressure"] = iaqSensor.pressure;
      messageJSON["BME680:rawHumidity"] = iaqSensor.rawHumidity;
      messageJSON["BME680:gasResistance"] = iaqSensor.gasResistance;
      messageJSON["BME680:stabStatus"] = iaqSensor.stabStatus;
      messageJSON["BME680:runInStatus"] = iaqSensor.runInStatus;
      messageJSON["BME680:temperature"] = iaqSensor.temperature;
      messageJSON["BME680:humidity"] = iaqSensor.humidity;
      messageJSON["BME680:gasPercentage"] = iaqSensor.gasPercentage;
      messageJSON["BME680:gasPercentageAccuracy"] = iaqSensor.gasPercentageAccuracy;

    } 
    updateBME680State();

    checkIaqSensorStatus(false);
    return haveNewData;
  }


  bool loopCCS811(){
    bool haveNewData = false; 
    //read data afterat interupt
  
    if (ccs.available()) {
      uint8_t error= ccs.readData();
      if (!error) {

        messageJSON["CCS811:eCO2"] = ccs.geteCO2();
        messageJSON["CCS811:TVOC"] = ccs.getTVOC();
        messageJSON["CCS811:RawResistance"]=ccs.getRawADCreading();
        haveNewData = true;
        checkIf30MinutesHavePassedCSS811(); //we calculate if the 30 minutes after activation have passed

      }
      else{
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
   //interrupt when data is ready
  void IRAM_ATTR ccs811Interrupt() {
     dataReadyCCS811 = true;
}



  // if bool atSetup=false, then we will terminate the loop of the device
  
  bool checkIaqSensorStatus(bool atSetup)
  {
    bool errorAppeared = false;
  
    if (iaqSensor.bsecStatus != BSEC_OK) {
      if (iaqSensor.bsecStatus < BSEC_OK) {
        output = "BSEC error code : " + String(iaqSensor.bsecStatus);
        Serial.println(output);
        errorAppeared = true;
        }
       else {
        output = "BSEC warning code : " + String(iaqSensor.bsecStatus);
        Serial.println(output);
      }
    }
   
    if (iaqSensor.bme68xStatus != BME68X_OK) {
      if (iaqSensor.bme68xStatus < BME68X_OK) {
        output = "BME68X error code : " + String(iaqSensor.bme68xStatus);
        Serial.println(output);
        errorAppeared = true;
      } else {
        output = "BME68X warning code : " + String(iaqSensor.bme68xStatus);
        Serial.println(output);
      }
    }
  
    if (errorAppeared == false){
      //Serial.println("Everything from BME680 is ok.");
      return true;
    }
    else{
      if (atSetup == true){
        return false;
      }
      else{
        Serial.println("Some error occured");
        errLeds();
        return false;
  
      }
  }
  }
  


void loadBME680State(void)
{
  if (EEPROM.read(0) == BSEC_MAX_STATE_BLOB_SIZE) {
    // Existing state in EEPROM
    Serial.println("Reading state from EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++) {
      bsecState[i] = EEPROM.read(i + 1);
      Serial.println(bsecState[i], HEX);
    }

  } else {
    // Erase the EEPROM with zeroes
    Serial.println("Erasing EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE + 1; i++)
    EEPROM.write(i, 0);

    EEPROM.commit();
  }
}

void updateBME680State(void)
{
  bool update = false;
  if (stateUpdateCounter == 0) {
    /* First state update when IAQ accuracy is >= 3 */
    if (iaqSensor.iaqAccuracy >= 3) {
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
    iaqSensor.getState(bsecState);
    checkIaqSensorStatus(false);

    Serial.println("Writing state to EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE ; i++) {
      EEPROM.write(i + 1, bsecState[i]);
      Serial.println(bsecState[i], HEX);
    }

    EEPROM.write(0, BSEC_MAX_STATE_BLOB_SIZE);
    EEPROM.commit();
  }
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
//Make a http get request to the server to get the environmental data of the room
void getEnvironmentalData(){
    HTTPClient http;
    Serial.println("Asking data from the server...");
    //default url + the specified endpoint
    createTheUrl(endpoint +"getEnvRoomData");
    Serial.println(serverUrl);
    http.begin(serverUrl);
    http.setTimeout(2000); // Set timeout to 2 seconds
    int httpResponseCode  = http.GET();
     if (httpResponseCode == HTTP_CODE_OK) {

        String stringResponse = http.getString();
        Serial.println(stringResponse);
        JsonDocument response;
        deserializeJson(response,stringResponse);
        Serial.println("Response:");
        temperature = response["temperature"];
        humidity = response["humidity"];
        Serial.print("Room Temperature: ");
        Serial.println(temperature);
        Serial.print("Room Humidity: ");  
        Serial.println(humidity);
        ccs.setEnvironmentalData(humidity, temperature);
        

    } else {
        Serial.print("POST failed. Error: ");
        Serial.println(http.errorToString(httpResponseCode).c_str());  // Get error description
           
    }
    firstTimeAskingEnvironmentalData = false; //we have asked the environmental data for the first time
    ENVIRONMENTAL_DATA_CCS811_TIMER = millis(); //set timer 
   // End HTTP connection
    http.end();
}


void sendErrorMessage(String error_message,uint8_t error){
  flashLeds();
 HTTPClient http;
  JsonDocument jsonError;
    Serial.println("Sending data to server...");
    createTheUrl(endpoint +"postError");

    Serial.println("The url is:"+(serverUrl));
    http.begin((serverUrl));//in case of adding more url sections into the url we send the post request

    http.addHeader("Content-Type", "application/json");
    http.setTimeout(2000);
    jsonError["error_number"] = error;
    // Serialize the  JSON
    String messageJSONString;
    serializeJsonPretty(jsonError, messageJSONString);
    Serial.println(messageJSONString);
    // Send POST request
    int httpResponseCode = http.POST(messageJSONString);
    Serial.println("The wifi is being:"+ String(WL_CONNECTED));
    // Print response
    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("Response:");
        Serial.println(response);
    } else {
        Serial.print("POST failed to arrive at server. Error: ");
        Serial.println(http.errorToString(httpResponseCode).c_str());  // Get error description
        
    }

    http.end();

}

