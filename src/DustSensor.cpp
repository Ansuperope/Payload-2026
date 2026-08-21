/**
 * @file DustSensor.cpp
 * @brief Implementation of HM3301 dust sensor module
 */

#include "DustSensor.h"

/**
 * @brief Initialize the HM3301 sensor
 *
 * The Seeed HM330X library handles communication
 * with the sensor.
 */
bool DustSensor::begin() {
  return sensorDust.init() == NO_ERROR;
}

/**
 * @brief Check checksum of received HM3301 data
 *
 * The HM3301 packet contains 29 bytes:
 *
 * Bytes 0-27  -> sensor data
 * Byte 28     -> checksum
 *
 * The checksum is the 8-bit sum of bytes 0-27.
 */
bool DustSensor::checkChecksum() {

  uint8_t sum = 0;

  for (uint8_t i = 0; i < 28; i++) {
    sum += dustBuffer[i];
  }

  return sum == dustBuffer[28];
}

/**
 * @brief Read and process HM3301 sensor data
 *
 * Reads the raw 29-byte packet from the sensor,
 * verifies the checksum, and extracts atmospheric
 * PM1.0, PM2.5, and PM10 values.
 */
DustPacket DustSensor::read() {

  DustPacket data;

  // Default values
  data.pm1_0 = 0;
  data.pm2_5 = 0;
  data.pm10 = 0;
  data.dustOK = false;
  data.errorCode = 0;

  /**
   * Read the raw sensor packet.
   *
   * The HM3301 returns 29 bytes of data.
   */
  if (sensorDust.read_sensor_value(dustBuffer, 29) != NO_ERROR) {
    data.errorCode = 1;  // Sensor read failure
    return data;
  }

  /**
   * Verify the received data.
   */
  if (!checkChecksum()) {
    data.errorCode = 2;  // Checksum failure
    return data;
  }

  /**
   * Extract atmospheric PM values.
   *
   * Each value is stored as two bytes:
   * high byte followed by low byte.
   */
  data.pm1_0 =
      ((uint16_t)dustBuffer[10] << 8) |
      dustBuffer[11];

  data.pm2_5 =
      ((uint16_t)dustBuffer[12] << 8) |
      dustBuffer[13];

  data.pm10 =
      ((uint16_t)dustBuffer[14] << 8) |
      dustBuffer[15];

  // Reading passed all checks
  data.dustOK = true;
  data.errorCode = 0;

  return data;
}