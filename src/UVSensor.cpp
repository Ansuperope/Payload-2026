/**
 * @file UVSensor.cpp
 * @brief Implementation of Adafruit LTR390 UV sensor module
 */

#include "UVSensor.h"

/**
 * @brief Initialize and configure the LTR390
 */
bool UVSensor::begin() {

  /**
   * Initialize the sensor over I2C.
   */
  if (!sensorLTR390.begin()) {
    return false;
  }

  /**
   * Configure the LTR390 for UV measurement.
   */
  sensorLTR390.setMode(LTR390_MODE_UVS);

  /**
   * Set the sensor gain.
   */
  sensorLTR390.setGain(LTR390_GAIN_3);

  /**
   * Set measurement resolution.
   */
  sensorLTR390.setResolution(LTR390_RESOLUTION_16BIT);

  /**
   * Enable the sensor.
   */
  sensorLTR390.enable(true);

  return true;
}

/**
 * @brief Read and process UV sensor data
 */
UVPacket UVSensor::read() {

  UVPacket data;

  // Default values
  data.uv = 0;
  data.uvOK = false;
  data.errorCode = 0;

  /**
   * Check whether a new measurement is available.
   */
  if (!sensorLTR390.newDataAvailable()) {
    data.errorCode = 1;  // Data not ready
    return data;
  }

  /**
   * Read the raw UV sensor value.
   *
   * IMPORTANT:
   * This is the raw UVS reading, not UV Index.
   */
  data.uv = sensorLTR390.readUVS();

  /**
   * Mark reading as valid.
   */
  data.uvOK = true;
  data.errorCode = 0;

  return data;
}