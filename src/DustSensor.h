/**
 * @file DustSensor.h
 * @brief Interface for HM3301 dust sensor module
 *
 * Handles initialization, reading, and error checking
 * for the Seeed HM3301 PM2.5 sensor.
 */

#ifndef DUST_SENSOR_H
#define DUST_SENSOR_H

#include <Arduino.h>
#include <Seeed_HM330X.h>

/**
 * @struct DustPacket
 * @brief Stores processed dust sensor data
 */
struct DustPacket {
  uint16_t pm1_0;     /**< PM1.0 concentration (ug/m^3) */
  uint16_t pm2_5;     /**< PM2.5 concentration (ug/m^3) */
  uint16_t pm10;      /**< PM10 concentration (ug/m^3) */
  bool dustOK;        /**< True if reading is valid */
  uint8_t errorCode;  /**< Error code (0 = no error) */
};

/**
 * @class DustSensor
 * @brief Class for interacting with the HM3301 sensor
 */
class DustSensor {
public:

  /**
   * @brief Initialize the HM3301 sensor
   * @return true if initialization is successful
   * @return false if initialization fails
   */
  bool begin();

  /**
   * @brief Read and process dust sensor data
   * @return DustPacket containing PM values and status
   */
  DustPacket read();

private:

  /** HM3301 sensor object */
  HM330X sensorDust;

  /**
   * HM3301 data packet.
   *
   * The HM3301 returns a 29-byte data packet.
   * Byte 28 contains the checksum.
   */
  uint8_t dustBuffer[29];

  /**
   * @brief Validate checksum of received data
   * @return true if checksum is valid
   * @return false if checksum is invalid
   */
  bool checkChecksum();
};

#endif