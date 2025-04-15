#ifndef structures
#define structures
//#pragma pack(1)
#include "enums.h"
#include "constants.h"
//It has a lot of variables the bme680
typedef struct informationFromBME680 {
    /**
      * @brief Index for Air Quality estimate [0-500]
      * 
      * Index for Air Quality (IAQ) gives an indication of the relative change in ambient TVOCs detected by BME680. 
      * 
      * @note The IAQ scale ranges from 0 (clean air) to 500 (heavily polluted air). During operation, algorithms 
      * automatically calibrate and adapt themselves to the typical environments where the sensor is operated 
      * (e.g., home, workplace, inside a car, etc.).This automatic background calibration ensures that users experience 
      * consistent IAQ performance. The calibration process considers the recent measurement history (typ. up to four 
      * days) to ensure that IAQ=50 corresponds to typical good air and IAQ=200 indicates typical polluted air.
      */
   float iaq;
   /**  
      *  @brief IAQ accuracy indicator will notify the user when she/he should initiate a calibration process. Calibration is 
      *   performed automatically in the background if the sensor is exposed to clean and polluted air for approximately 
      *   30 minutes each.
      * 
      *   | Virtual sensor             | Value |  Accuracy description                                                                                                                                         |
      *   |----------------------------|-------|---------------------------------------------------------------------------------------------------------------------------------------------------------------|
      *   | IAQ                        |   0   | Stabilization / run-in ongoing                                                                                                                                |
      *   |                            |   1   | Low accuracy,to reach high accuracy(3),please expose sensor once to good air (e.g. outdoor air) and bad air (e.g. box with exhaled breath) for auto-trimming  |
      *   |                            |   2   | Medium accuracy: auto-trimming ongoing                                                                                                                        |
      *   |                            |   3   | High accuracy                                                                                                                                                 |
      */     
   int iaqAccuracy;
 
   // Unscaled Index for Air Quality estimate
   float staticIaq;
 
   //CO2 equivalent estimate [ppm]
   float co2Equivalent;
 
   //Breath VOC concentration estimate [ppm]
   float breathVocEquivalent;
 
    /**
      * @brief Temperature sensor signal [degrees Celsius]
      * 
      * Temperature directly measured by BME680 in degree Celsius. 
      * 
      * @note This value is cross-influenced by the sensor heating and device specific heating.
     */
   float rawTemperature;
 
    /**
      * @brief Pressure sensor signal [Pa]
      * 
      * Pressure directly measured by the BME680 in Pa.
      */
   float pressure;
 
    /**
      * @brief Relative humidity sensor signal [%] 
      * 
      * Relative humidity directly measured by the BME680 in %.  
      * 
      * @note This value is cross-influenced by the sensor heating and device specific heating.
      */  
   float rawHumidity;
 
   /**
      * @brief Gas sensor signal [Ohm]
      * 
      * Gas resistance measured directly by the BME680 in Ohm.The resistance value changes due to varying VOC 
      * concentrations (the higher the concentration of reducing VOCs, the lower the resistance and vice versa). 
   */
   float gasResistance;
 
   /**
      * @brief Gas sensor stabilization status [boolean]
      * 
      * Indicates initial stabilization status of the gas sensor element: stabilization is ongoing (0) or stabilization 
      * is finished (1).   
   */
   float stabStatus;
 
   /**
      * @brief Gas sensor stabilization status [boolean]
      * 
      * Indicates initial stabilization status of the gas sensor element: stabilization is ongoing (0) or stabilization 
      * is finished (1).   
   */
   float runInStatus;
 
   /**
      * @brief Sensor heat compensated temperature [degrees Celsius]
      * 
      * Temperature measured by BME680 which is compensated for the influence of sensor (heater) in degree Celsius. 
      * The self heating introduced by the heater is depending on the sensor operation mode and the sensor supply voltage. 
      * 
      * 
      * @note IAQ solution: In addition, the temperature output can be compensated by an user defined value 
      * (::BSEC_INPUT_HEATSOURCE in degrees Celsius), which represents the device specific self-heating.
      * 
      * Thus, the value is calculated as follows:
      * * IAQ solution: ```BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE = ::BSEC_INPUT_TEMPERATURE -  function(sensor operation mode, sensor supply voltage) - ::BSEC_INPUT_HEATSOURCE```
      * * Other solutions: ```::BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE = ::BSEC_INPUT_TEMPERATURE -  function(sensor operation mode, sensor supply voltage)```
      *
      * The self-heating in operation mode #BSEC_SAMPLE_RATE_ULP is negligible.
      * The self-heating in operation mode #BSEC_SAMPLE_RATE_LP is supported for 1.8V by default (no config file required). If the BME680 sensor supply voltage is 3.3V, the corresponding config file shall be used.
      */
   float temperature;
 
  /**
      * @brief Sensor heat compensated humidity [%] 
      * 
      * Relative measured by BME680 which is compensated for the influence of sensor (heater) in %.
      * 
      * It converts the ::BSEC_INPUT_HUMIDITY from temperature ::BSEC_INPUT_TEMPERATURE to temperature 
      * ::BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE.
      * 
      * @note IAQ solution: If ::BSEC_INPUT_HEATSOURCE is used for device specific temperature compensation, it will be 
      * effective for ::BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY too.
      */
   float humidity;
   
   /*!< Percentage of min and max filtered gas value [%] */
   float gasPercentage;
 
 
 }informationFromBME680;


 typedef struct informationFromCCS811 {
    /**************************************************************************/
 /*!
     @brief  returns the stored estimated carbon dioxide measurement. This does
    does not read the sensor. To do so, call readData()
     @returns eCO2 measurement as 16 bit integer
 */
 /**************************************************************************/
 uint32_t eCO2;
 /**************************************************************************/
 /*!
     @brief  returns the stored total volatile organic compounds measurement.
    This does does not read the sensor. To do so, call readData()
     @returns TVOC measurement as 16 bit integer
 */
 /**************************************************************************/
 uint32_t TVOC;
}informationFromCCS811;

union informationFromSensorUnion {
      
  informationFromBME680 sensorBME680;
  informationFromCCS811 sensorCCS811;
};

typedef struct sensorMessage {
  int id;
  recognized_Sensor sensor;
  union informationFromSensorUnion informationFromSensor;

}sensorMessage;

extern sensorMessage message;

//The queue at receiver device that keeps the messages containing the sensors information of all devices, even itself
typedef struct MessageQueue {
  sensorMessage *messages;  // Δείκτης προς δυναμικά δεσμευμένη μνήμη
  int maxSize;              // Μέγιστο μέγεθος ουράς
  int front;                 // Index πρώτου στοιχείου
  int rear;                  // Index επόμενου στοιχείου
  int currentSize;                 // Τρέχον πλήθος στοιχείων
} MessageQueue;

//Set at customQueue.cpp only for the purpose of avoiding taking too much space
extern MessageQueue receiverQueue;

//The response message that the receiver sends to the sender devices after it read the confirmation back from the python script
typedef struct ResponseMessageFromReceiver {
  int id;
  bool writtenIntoQueue;
}ResponseMessageFromReceiver;


//Just in case
typedef struct ErrorMessage{
  int id;
  char* error;
}ErrorMessage;


#endif
