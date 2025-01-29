
#ifndef constants
#define constants
#include <stdint.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <esp_now.h>

extern  uint8_t my_MAC[6];

extern const int ESP32_TOTAL_DEVICES_NUMBER;

extern const uint8_t ESP32_MAC_OF_WHITE_BREADBOARD[6];
extern const uint8_t ESP32_MAC_OF_TRANSPARENT_BREADBOARD[6];
extern const uint8_t ESP32_MAC_OF_YELLOW_BREADBOARD[6];
extern const uint8_t *MAC_LIBRARY[];
uint8_t ESP32_MAC_OF_RECEIVER[6];


extern const int ESP32_ID_WHITE_BREADBOARD;
extern const int ESP32_ID_TRANSPARENT_BREADBOARD;
extern const int ESP32_ID_YELLOW_BREADBOARD;

typedef struct SensorMessage{
/*Id of the microcontroller having the sensor*/
  uint8_t id;
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
  /*
  Calculates the altitude (in meters).
 *          Reads the current atmostpheric pressure (in hPa) from the sensor and
 * calculates via the provided sea-level pressure (in hPa).
 * */
  float altitude;
}SensorMessage;

extern SensorMessage myData;
extern uint8_t id;

extern const uint8_t BME_SCK;
extern const uint8_t BME_MISO;
extern const uint8_t BME_MOSI;
extern const uint8_t BME_CS;
const float SEALEVELPRESSURE_HPA = SEALEVELPRESSURE_HPA (1013.25);
esp_now_peer_info_t peerInfo;


#endif