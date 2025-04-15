#include "custom_headers.h"
#include "customFunctions.h"


//Get the size of the structs we have inside the union that repressents the payload of each sensor
int getByteSizeOfTypeOfSensor(const recognized_Sensor sensor){
  int byteSize=0;
  switch (sensor){
    case BME680:
      byteSize = sizeof(informationFromBME680);
      break;
    case CCS811: 
    byteSize = sizeof(informationFromCCS811);
      break; 
    default:
      Serial.println("Sensor aren't being recognized");
      errLeds();
  }
  return byteSize;
}


bool setupBME680(){
  
    iaqSensor.begin(BME68X_I2C_ADDR_HIGH, Wire);
    output = "\nBSEC library version " + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
   // Serial.println(output);
    //Perhaps the bme680 doesn't exist so we return false
    if (checkIaqSensorStatus(true) == false){
      return false;
    }
  
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
  
    iaqSensor.updateSubscription(sensorList, 13, BSEC_SAMPLE_RATE_CONT);
    checkIaqSensorStatus(false);
  
    // Print the header
    output = "Timestamp [ms], IAQ, IAQ accuracy, Static IAQ, CO2 equivalent, breath VOC equivalent, raw temp[°C], pressure [hPa], raw relative humidity [%], gas [Ohm], Stab Status, run in status, comp temp[°C], comp humidity [%], gas percentage";
    //Serial.println(output);
    return true;
  }
  
  
  
  bool setupCCS811(){
    //Serial.println("CCS811 test");
  
    if(!ccs.begin()){
      //Serial.println("Failed to start sensor! Please check your wiring.");
      return false;
    }
  
    // Wait for the sensor to be ready
    while(!ccs.available());
    return true;
  }
  
  void loopSensor(sensorMessage *message){
      if (message->sensor == BME680){
         loopBME680(message);
      }
      if (message->sensor == CCS811){
         loopCCS811(message);
      }
      setTimer();
      setFlag(WAIT_FOR_RESPONSE);
  }
  
  void loopBME680(sensorMessage *message){
    if (iaqSensor.run()) { // If new data is available
     message->informationFromSensor.sensorBME680.iaq  = iaqSensor.iaq;
     message->informationFromSensor.sensorBME680.iaqAccuracy  = iaqSensor.iaqAccuracy;
     message->informationFromSensor.sensorBME680.staticIaq  = (int) iaqSensor.staticIaq;
     message->informationFromSensor.sensorBME680.co2Equivalent  = iaqSensor.co2Equivalent;
     message->informationFromSensor.sensorBME680.breathVocEquivalent  = iaqSensor.breathVocEquivalent;
     message->informationFromSensor.sensorBME680.rawTemperature  = iaqSensor.rawTemperature;
     message->informationFromSensor.sensorBME680.pressure  = iaqSensor.pressure;
     message->informationFromSensor.sensorBME680.rawHumidity  = iaqSensor.rawHumidity;
     message->informationFromSensor.sensorBME680.gasResistance  = iaqSensor.gasResistance;
     message->informationFromSensor.sensorBME680.stabStatus  = iaqSensor.stabStatus;
     message->informationFromSensor.sensorBME680.runInStatus  = iaqSensor.runInStatus;
     message->informationFromSensor.sensorBME680.temperature  = iaqSensor.temperature;
     message->informationFromSensor.sensorBME680.humidity  = iaqSensor.humidity;
     message->informationFromSensor.sensorBME680.gasPercentage  = iaqSensor.gasPercentage;
    } 
    checkIaqSensorStatus(false);
  }


  void loopCCS811(sensorMessage *message){
    if(ccs.available()){
      if(!ccs.readData()){
        message->informationFromSensor.sensorCCS811.eCO2 =(uint32_t)  ccs.geteCO2();
        message->informationFromSensor.sensorCCS811.TVOC =(uint32_t)  ccs.getTVOC();
      }
      else{
        Serial.println("Error at CSS811");
        errLeds();
      }
    }
  }

   
  
  // if bool atSetup=false, then we will terminate the loop of the device
  
  bool checkIaqSensorStatus(bool atSetup)
  {
    bool errorAppeared = false;
  
    if (iaqSensor.bsecStatus != BSEC_OK) {
      if (iaqSensor.bsecStatus < BSEC_OK) {
        output = "BSEC error code : " + String(iaqSensor.bsecStatus);
        Serial.println(output);
        bool errorAppeared = true;
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
        bool errorAppeared = true;
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
  

  void printMessageInformation(const sensorMessage message){

    Serial.println("The information of the sensor message are:");
    output = "id:"+ String(message.id)+ "and sensor type:" + enum_recognized_Sensor_to_Strings( message.sensor)+ "Saved as" +String( message.sensor);
    Serial.print(output);
    if (message.sensor == BME680) {
      printBME680messageInformation(message.informationFromSensor.sensorBME680);
    }
    else if(message.sensor == CCS811){
      printCCS811messageInformation(message.informationFromSensor.sensorCCS811);
    }
    

    
  }

  void printBME680messageInformation(const informationFromBME680 sensorBME680){
    Serial.println("The information of the BME680 sensor message are:");
    output = "iaq:"+ String(sensorBME680.iaq)+ ".";
    Serial.print(output);
    output = "iaqAccuracy:"+ String(sensorBME680.iaqAccuracy)+ ".";
    Serial.print(output);
    output = "staticIaq:"+ String(sensorBME680.staticIaq)+ ".";
    Serial.print(output);
    output = "co2Equivalent:"+ String(sensorBME680.co2Equivalent)+ ".";
    Serial.print(output);
    output = "breathVocEquivalent:"+ String(sensorBME680.breathVocEquivalent)+ ".";
    Serial.print(output);
    output = "rawTemperature:"+ String(sensorBME680.rawTemperature)+ ".";
    Serial.print(output);
    output = "pressure:"+ String(sensorBME680.pressure)+ ".";
    Serial.print(output);
    output = "rawHumidity:"+ String(sensorBME680.rawHumidity)+ ".";
    Serial.print(output);
    output = "gasResistance:"+ String(sensorBME680.gasResistance)+ ".";
    Serial.print(output);
    output = "stabStatus:"+ String(sensorBME680.stabStatus)+ ".";
    Serial.print(output);
    output = "runInStatus:"+ String(sensorBME680.runInStatus)+ ".";
    Serial.print(output);
    output = "temperature:"+ String(sensorBME680.temperature)+ ".";
    Serial.print(output);
    output = "humidity:"+ String(sensorBME680.humidity)+ ".";
    Serial.print(output);
    output = "gasPercentage:"+ String(sensorBME680.gasPercentage)+ ".";
    Serial.print(output);

}
void printCCS811messageInformation(const informationFromCCS811 sensorCCS811){

    output = "humidity:"+ String(sensorCCS811.eCO2)+ ".";
    Serial.print(output);
    output = "gasPercentage:"+ String(sensorCCS811.TVOC)+ ".";
    Serial.print(output);

}

String enum_recognized_Sensor_to_Strings(recognized_Sensor sensor){
  String response;
  switch (sensor){
    case NO_KNOWN_SENSOR:
        response = "NO_KNOWN_SENSOR";
        break;
    case BME680:
        response = "BME680";
        break;

    case CCS811:
       response = "CSS811";
       break;
    default:
      response = "INVALID POSITION";
  }
  return response;
}

