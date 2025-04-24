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
    output = "\nBSEC library version " + String(iaqSensor.version.major) + "," + String(iaqSensor.version.minor) + "," + String(iaqSensor.version.major_bugfix) + "," + String(iaqSensor.version.minor_bugfix);
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
    Serial.println(String(haveNewData));
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
      messageJSON["BME680:breathVocEquivalent"] = iaqSensor.breathVocEquivalent;
      messageJSON["BME680:rawTemperature"] = iaqSensor.rawTemperature;
      messageJSON["BME680:pressure"] = iaqSensor.pressure;
      messageJSON["BME680:rawHumidity"] = iaqSensor.rawHumidity;
      messageJSON["BME680:gasResistance"] = iaqSensor.gasResistance;
      messageJSON["BME680:stabStatus"] = iaqSensor.stabStatus;
      messageJSON["BME680:runInStatus"] = iaqSensor.runInStatus;
      messageJSON["BME680:temperature"] = iaqSensor.temperature;
      messageJSON["BME680:humidity"] = iaqSensor.humidity;
      messageJSON["BME680:gasPercentage"] = iaqSensor.gasPercentage;

    } 
    checkIaqSensorStatus(false);
    return haveNewData;
  }


  bool loopCCS811(){
    bool haveNewData = false; 
    if(ccs.available()){
      if(!ccs.readData()){
        messageJSON["CCS811eCO2"] = ccs.geteCO2();
        messageJSON["CCS811TVOC"] = ccs.getTVOC();
        haveNewData = true;
        }
      else{
        Serial.println("Error at CSS811");
        errLeds();
      }
    }
    return haveNewData;
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
    output = "id:"+ String(message.id)+ ","+ "sensor:" +"\"" +enum_recognized_Sensor_to_Strings( message.sensor)+ "\"" +",";
    Serial.print(output);
    if (message.sensor == BME680) {
      printBME680messageInformation(message.informationFromSensor.sensorBME680);
    }
    else if(message.sensor == CCS811){
      printCCS811messageInformation(message.informationFromSensor.sensorCCS811);
    }
    Serial.println();    
  }


  void loadBME680state(){

  }
  void getBME680state(){

  }

  void printBME680messageInformation(const informationFromBME680 sensorBME680){
    output = "iaq:"+ String(sensorBME680.iaq)+ ",";
    Serial.print(output);
    output = "iaqAccuracy:"+ String(sensorBME680.iaqAccuracy)+ ",";
    Serial.print(output);
    output = "staticIaq:"+ String(sensorBME680.staticIaq)+ ",";
    Serial.print(output);
    output = "co2Equivalent:"+ String(sensorBME680.co2Equivalent)+ ",";
    Serial.print(output);
    output = "breathVocEquivalent:"+ String(sensorBME680.breathVocEquivalent)+ ",";
    Serial.print(output);
    output = "rawTemperature:"+ String(sensorBME680.rawTemperature)+ ",";
    Serial.print(output);
    output = "pressure:"+ String(sensorBME680.pressure)+ ",";
    Serial.print(output);
    output = "rawHumidity:"+ String(sensorBME680.rawHumidity)+ ",";
    Serial.print(output);
    output = "gasResistance:"+ String(sensorBME680.gasResistance)+ ",";
    Serial.print(output);
    output = "stabStatus:"+ String(sensorBME680.stabStatus)+ ",";
    Serial.print(output);
    output = "runInStatus:"+ String(sensorBME680.runInStatus)+ ",";
    Serial.print(output);
    output = "temperature:"+ String(sensorBME680.temperature)+ ",";
    Serial.print(output);
    output = "humidity:"+ String(sensorBME680.humidity)+ ",";
    Serial.print(output);
    output = "gasPercentage:"+ String(sensorBME680.gasPercentage)+ ",";
    Serial.print(output);

}
void printCCS811messageInformation(const informationFromCCS811 sensorCCS811){

    output = "humidity:"+ String(sensorCCS811.eCO2)+ ",";
    Serial.print(output);
    output = "gasPercentage:"+ String(sensorCCS811.TVOC)+ ",";
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

