#include "custom_headers.h"
#include "customFunctions.h"

//Initialize the values of the json object to zero

void addMessageToBuffer(){
  JsonDocument messageJSON;
  
  messageJSON = createIndividualMessage();
  Serial.println("Adding message to buffer");


  if (buffer.size() <= maxBufferSize) {

      buffer.add(messageJSON);  
  }
  else {
      buffer[currentNumberOfUnsendedData % maxBufferSize] = messageJSON; // Replace the oldest message
      Serial.println("Buffer is full, replacing the oldest message");
  }
   
  currentNumberOfUnsendedData++;
  Serial.println("The number of unsent data is "+ String(currentNumberOfUnsendedData));
  Serial.println("Message added to buffer");
  ramAvailable();
}

 

JsonDocument createIndividualMessage(){
    JsonDocument messageJSON;
    messageJSON["timestamp"] = getLocalDateTime();
    messageJSON["Id"] = id;
    messageJSON["Sensor"] = sensorLocatedIntoDevice;
     if (sensorLocatedIntoDevice =="BME680"){
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
  else
  {
  if (sensorLocatedIntoDevice == "CCS811"){
         messageJSON["CCS811:eCO2"] = ccs.geteCO2();
        messageJSON["CCS811:TVOC"] = ccs.getTVOC();
        messageJSON["CCS811:RawResistance"]=ccs.getRawADCreading();
        checkIf30MinutesHavePassedCSS811(); //we calculate if the 30 minutes after activation have passed
  }
}
    return messageJSON;
}

bool clearBufferAndCounter(){
    buffer.clear(); // Clear the buffer
    if (buffer.size() == 0) {
        currentNumberOfUnsendedData = 0; // Reset the counter

        Serial.println("Buffer cleared successfully.");
        return true; // Buffer cleared successfully
    } else {
        Serial.println("Failed to clear buffer.");
        return false; // Failed to clear buffer
    }
}
