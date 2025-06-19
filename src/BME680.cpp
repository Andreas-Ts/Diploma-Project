#include "custom_headers.h"






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
        sendErrorMessage(output,iaqSensor.bme68xStatus);
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

