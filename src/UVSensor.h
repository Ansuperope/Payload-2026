/**
 * @file UVSensor.h
 * @brief Interface for Adafruit LTR390 UV sensor module
 *
 * Handles initialization, reading, and error checking
 * for the Adafruit LTR390 UV sensor.
 */

#ifndef UV_SENSOR_H
#define UV_SENSOR_H

#include <Arduino.h>
#include <Adafruit_LTR390.h>

/**
 * @struct UVPacket
 * @brief Stores processed LTR390 UV sensor data
 */
struct UVPacket {

  /**
   * Raw UV sensor reading.
   *
   * This is the raw LTR390 UVS ADC value,
   * NOT the UV Index.
   */
  uint32_t uv;

  /** True if the UV reading is valid */
  bool uvOK;

  /**
   * Error code:
   * 0 = no error
   * 1 = data not ready
   */
  uint8_t errorCode;
};

/**
 * @class UVSensor
 * @brief Class for interacting with the Adafruit LTR390 sensor
 */
class UVSensor {
public:

  /**
   * @brief Initialize and configure the LTR390
   * @return true if initialization succeeds
   * @return false if sensor is not detected
   */
  bool begin();

  /**
   * @brief Read UV sensor data
   * @return UVPacket containing raw UV data and status
   */
  UVPacket read();

private:

  /** Adafruit LTR390 sensor object */
  Adafruit_LTR390 sensorLTR390;
};

#endif