
#include "custom_headers.h"
#include "customFunctions.h"

void initializemessageJSON(){
  messageJSON["DateTime"] = nullptr;
  messageJSON["Id"] = id;
  messageJSON["Sensor"] = sensorLocatedIntoDevice;
  messageJSON["BME680:iaq"] = nullptr;
  messageJSON["BME680:iaqAccuracy"] = nullptr;
  messageJSON["BME680:staticIaq"] = nullptr;
  messageJSON["BME680:co2Equivalent"] = nullptr;
  messageJSON["BME680:breathVocEquivalent"] = nullptr;
  messageJSON["BME680:rawTemperature"] = nullptr;
  messageJSON["BME680:pressure"] = nullptr;
  messageJSON["BME680:rawHumidity"] = nullptr;
  messageJSON["BME680:gasResistance"] = nullptr;
  messageJSON["BME680:stabStatus"] = nullptr;
  messageJSON["BME680:runInStatus"] = nullptr;
  messageJSON["BME680:temperature"] = nullptr;
  messageJSON["BME680:humidity"] = nullptr;
  messageJSON["BME680:gasPercentage"] = nullptr;
  messageJSON["CCS811:eCO2"] = nullptr;
  messageJSON["CCS811:TVOC"] = nullptr;

}


bool sendDataToSerial(const sensorMessage messageToBeInsertedToSerial){
 
  unsigned int bytesSend = 0;
  int sensorMessageSize = sizeof(sensorMessage);
  // Serial.println(messageToBeInsertedToSerial.id);
   uint8_t *dataToBeSentToSerial = (uint8_t*) malloc(sensorMessageSize);
    if (dataToBeSentToSerial == NULL){
      Serial.println("Error: malloc failed");
      errLeds();
      return false;
    }
    memcpy(dataToBeSentToSerial,&messageToBeInsertedToSerial,sensorMessageSize);
    bytesSend += Serial.write(dataToBeSentToSerial, sensorMessageSize);  // Send id 
    Serial.flush();

    free(dataToBeSentToSerial);
    if (bytesSend>8) { //metadata and payload send
        return true;
    }else{
        return false; //either no data was sent or only metadata
    } 
  }
  

bool checkForInactivityOverThreshold(const unsigned long timeLastMessageWasSend,unsigned long threshold){
    unsigned long current_time = millis();
    unsigned long remaining_time = current_time - timeLastMessageWasSend ;
    if (remaining_time > threshold){
        return true;
    }
    else {
        return false;
    }
}

//keep a minimum time in order to send a new message
bool isTimeToSendMessage(Setting settingOfSensor,const unsigned long timeLastMessageWasSend){
  unsigned long currentTime = millis();
  unsigned long minimumTimeToPass;
  switch (settingOfSensor){
      case(NO_WAIT):
        minimumTimeToPass = frequencyMinimum;
        break;
      case(WAIT_A_BIT):
        minimumTimeToPass = frequencyWhenFailureOccurs;
        break;
      default:   
        Serial.println("Some error occured");
        errLeds();
  }
  
  if (abs((double)(currentTime - timeLastMessageWasSend)) > minimumTimeToPass){
        //time to send the message
        
        return true;
  }
  else{
        return false;
 }
}



//Set the flag enum that the sensor has. It could change in some flags the time of the last message that was sent

void setFlag(Setting setting){
  settingOfSensor = setting;

}

void setTimer(){
  timeLastMessageWasSend = millis();


}
ResponseMessageFromReceiver createResponseFromReceiver(const int sensorMessage,bool writtenIntoQueue,bool writtenIntoPython){

  ResponseMessageFromReceiver response;
  response.id = id;
  response.writtenIntoQueue = writtenIntoQueue;
  return response;
}


/* Halt in case of failure */
void errLeds(void)
{
  for (;;){
    flashLeds();
  }
}
/*For debugging reasons*/

void flashLeds(){
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}