#include <stdint.h>

#ifndef macros.h
#define macros.h

extern const uint8_t ESP32_MAC_OF_WHITE_BREADBOARD[6];
extern const uint8_t ESP32_MAC_OF_TRANSPARENT_BREADBOARD[6];
extern const uint8_t ESP32_MAC_OF_YELLOW_BREADBOARD[6];
extern  uint8_t ESP32_MAC_OF_RECEIVER[6];

extern const int ESP32_ID_WHITE_BREADBOARD;
extern const int ESP32_ID_TRANSPARENT_BREADBOARD;
extern const int ESP32_ID_YELLOW_BREADBOARD;

typedef struct {
/*Id of the microcontroller having the sensor*/
  uint32_t id;
    /** Temperature (Celsius) assigned after calling performReading() or
   * endReading() **/
  float temperature;
  /** Pressure (Pascals) assigned after calling performReading() or endReading()
   * **/
  uint32_t pressure;
  /** Humidity (RH %) assigned after calling performReading() or endReading()
   * **/
  float humidity;
  /** Gas resistor (ohms) assigned after calling performReading() or
   * endReading() **/
  uint32_t gas_resistance;
} SensorMessage;

extern SensorMessage Sensor_message;

#endif