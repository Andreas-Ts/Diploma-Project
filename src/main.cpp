
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <math.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)
int loop_counter = 0 ;
int counter = millis(); 
Adafruit_BME680 bme; // I2C
//Adafruit_BME680 bme(BME_CS); // hardware SPI
//Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println(F("BME680 async test"));

  if (!bme.begin()) {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
    while (1);
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}

void loop() {
  
  // Tell BME680 to begin measurement.
  
  // Serial.print("counter=");
  //Serial.println(counter);
  unsigned long timetook;
  timetook=millis();
  
   unsigned long timestart =millis();
  if (! bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
   unsigned long timeend = millis();
   
  Serial.print(F("It took "));
  Serial.print(timetook);
  Serial.println(" milliseconds");

 
 
  
  unsigned long endTime = bme.beginReading();
  
  unsigned long willtake = endTime - timetook  ;
  if (endTime == 0) {
    Serial.println(F("Failed to begin reading :("));
    return;
  }
  
  Serial.print(F("Reading started at "));
  Serial.print(timetook);
  Serial.print(F(",it will take "));
  Serial.print(willtake);
  Serial.print(F(" and will finish at "));
  Serial.println(endTime);

  Serial.println(F("You can do other work during BME680 measurement."));
   // This represents parallel work.
  // There's no need to delay() until millis() >= endTime: bme.endReading()
  // takes care of that. It's okay for parallel work to take longer than
  // BME680's measurement time.

  // Obtain measurement results from BME680. Note that this operation isn't
  // instantaneous even if milli() >= endTime due to I2C/SPI latency.
  if (!bme.endReading()) {
    Serial.println(F("Failed to complete reading :("));
    return;
  }

  timetook= abs((int) (millis() - endTime) ) ;
  Serial.print(F("Reading completed at "));
  Serial.println(millis());
   Serial.print(F(" and it actually took "));
  Serial.println(timetook);
 
  Serial.print(F("Temperature = "));
  Serial.print(bme.temperature);
  Serial.println(F(" *C"));

  Serial.print(F("Pressure = "));
  Serial.print(bme.pressure / 100.0);
  Serial.println(F(" hPa"));

  Serial.print(F("Humidity = "));
  Serial.print(bme.humidity);
  Serial.println(F(" %"));

  Serial.print(F("Gas = "));
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(F(" KOhms"));

  Serial.print(F("Approx. Altitude = "));
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(F(" m"));
  
  int aba = millis() - counter;
 /*
  Serial.print("aba=");
  Serial.println(aba);
  loop_counter++;
  */
  if (aba < 1000){
    /*
     Serial.print("We are at ");
     Serial.print(millis());
     Serial.println(" milliseconds.");
    */
  }
  else{
      Serial.print("We are at ");
      Serial.print(millis());
      Serial.println(" milliseconds.");
      Serial.print(F("We had "));
      Serial.print(loop_counter);
      Serial.println(F(" repetations of measurements"));
      delay(10 * 1000);
      loop_counter = 0 ;
      counter = millis(); 
  }

  Serial.println();


  

}
