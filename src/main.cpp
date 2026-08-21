/**
 * @file main.cpp
 * @details Main code, what will be uploaded to microcontroller. This is
 *          assuming the component is a Teensy
 */
#ifndef MAIN_CPP
#define MAIN_CPP
/***********************************************************************
 * LIBRARIES
 **********************************************************************/
#include <Arduino.h>  // To make it compatible for Platform.IO
                      // delete if you're using Arduino IDE
#include <SD.h>       // Teensy SD config
#include <Wire.h>     // Teensy for wire configs

// OUR CUSTOM LIBRARY 
#include <CSV.h>      // CSV files

// SENSOR LIBRARIES
#include <Adafruit_BME280.h>    // humidity: https://github.com/boschsensortec/BME280_SensorAPI
#include <Seeed_HM330X.h>       // dust: https://github.com/Seeed-Studio/Seeed_PM2_5_sensor_HM3301
#include <Adafruit_LTR390.h>    // uv: https://github.com/adafruit/Adafruit_LTR390/tree/master
#include "DustSensor.h"
#include "UVSensor.h"


#endif

/***********************************************************************
 * VARAIBLES TO CHANGE : rates, serials, etc.
 * Things that we might need to change
 **********************************************************************/
// SIZES: size = send rate (second) * read rate (Hz)
const uint8_t SIZE_BUFFER_RAM = 10;

// RATES: rate (millis) = 1000 millis /  times to run per second
const uint8_t RATE_BUFFER_RAM = 100;   // 10Hz, read sensor data
const uint8_t RATE_SD = 1000;          // 1Hz, save to sd

// SENSOR RELATED
const float SEA_LEVEL = 1013.25;       // sealevel pressure in hPa

/***********************************************************************
 * CONTANTS AND VARIABLES
 **********************************************************************/
// CSV FILES
CSV fileHumid("humidity.csv");
CSV fileDust("dust.csv"); // added for dust sensor
CSV fileUV("uv.csv"); // added for uv sensor

// SENSOR OBJECTS
Adafruit_BME280 sensorHumid; // humidity, temp, pressure
DustSensor sensorDust;     // dust sensor 
UVSensor sensorUV;         // uv sensor

// COUNTER
uint8_t counter_ram = 0;    // index for ram buffer
uint8_t indexSave;          // index for saving to sd

// VARIABLES FOR KEEPING TRACK OF TIME
float currentTime;    // current time, from when sensor was turned on
float lastBufferRam;  // last time sensor was read
float lastSDSave;     // last time data was saved to sd

/***********************************************************************
 * PACKETS
 **********************************************************************/
// Packet for RAM buffer - general sensors
struct packetRam {
  float ramTime;   // time read data

  // bme sensor
  float bmeAltitude;
  float bmeHumidity;
  float bmePressure; 
  float bmeTemp;

   // ***************** DUST *****************
  uint16_t dustPM1_0; /**< PM1.0 (ug/m^3) */
  uint16_t dustPM2_5; /**< PM2.5 (ug/m^3) */
  uint16_t dustPM10;  /**< PM10 (ug/m^3) */
  bool dustOK;        /**< Valid reading */
  uint8_t dustError;  /**< Error code */
}; // END packetRam


// ***************** DUST *****************
// Packet for UV sensor
struct packetUV {
  float uvTime;       // time read data
  float uvIndex;      // UV index
  float uvIlluminance; // UV illuminance
  bool uvOK;          // Valid reading
  uint8_t uvError;    // Error code
}; // END packetUV

/***********************************************************************
 * BUFFERS
 **********************************************************************/
// Buffer for all sensors
packetRam BufferRam[SIZE_BUFFER_RAM];

/***********************************************************************
 * CODE THAT RUNS ONCE
 **********************************************************************/
void setup() {
  // I2C CONNECTIONS
  Wire.begin();     // Default

  // UART CONNECTIONS
  Serial.begin(115600);

  // CONFIGS FOR TRANSMITTING OR RECEIVING

  /*********************************************************************
   * CHECK IF SENSORS ARE CONNECTED HERE
   ********************************************************************/
  // CHECK IF BME: humidity, temp, and pressure
   if (!sensorHumid.begin()) {
    Serial.println("Error! Cannot find sensor: BME");
  }
  // sensor code

  /*********************************************************************
   * CREATE CSV FILES
   ********************************************************************/
  fileHumid.createCSV("time,altitude,humidity,pressure,temperature");
  
  // DELAY, milli seconds
  delay(300);

} // END setup() -------------------------------------------------------



/***********************************************************************
 * CODE THAT WILL RUN CONTINUOUSLY
 * Make faster tasks first and slower tasks last
 **********************************************************************/
void loop() {
  currentTime = millis();

  /*********************************************************************
   * RAM BUFFER: READING SENSOR DATA
   * -------------------------------------------------------------------
   * Condition:
   *  if its time to read AND if our buffer isnt full
   * 
   * For sensors / data:
   *  - 
   ********************************************************************/
  if (currentTime - lastBufferRam >= RATE_BUFFER_RAM) {

    // SAVE TIME
    BufferRam[counter_ram].ramTime = currentTime;

    /*******************************************************************
     * READ BME DATA: temp, pressure, humidity
     ******************************************************************/
    BufferRam[counter_ram].bmeAltitude = sensorHumid.readAltitude(SEA_LEVEL);
    BufferRam[counter_ram].bmeHumidity = sensorHumid.readHumidity();
    BufferRam[counter_ram].bmePressure = sensorHumid.readPressure();
    BufferRam[counter_ram].bmeTemp = sensorHumid.readTemperature();

    // INPUT CODE TO READ SENSORS
    DustPacket dust = sensorDust.read();

    BufferRam[counter_ram].dustPM1_0 = dust.pm1_0;
    BufferRam[counter_ram].dustPM2_5 = dust.pm2_5;
    BufferRam[counter_ram].dustPM10 = dust.pm10;
    BufferRam[counter_ram].dustOK = dust.dustOK;
    BufferRam[counter_ram].dustError = dust.errorCode;
    
    // INCREMENT INDEX FOR NEXT READ
    counter_ram++;
    lastBufferRam = currentTime;
    
  } // END if

  /*********************************************************************
   * SAVING TO SD
   * -------------------------------------------------------------------
   * Note: code is assuming transmitting and sd saving rate are the same
   * 
   * Condition:
   *  if its time to save AND if our buffer is full
   * 
   * For sensors / data:
   *  -
   ********************************************************************/
  if (currentTime - lastSDSave >= RATE_SD) {
    uint8_t indexSave = 0;

    while (indexSave < SIZE_BUFFER_RAM) {

      // WRITE TO FILE
      fileHumid.writeToFile(BufferRam[indexSave].ramTime,
                            BufferRam[indexSave].bmeAltitude,
                            BufferRam[indexSave].bmeHumidity,
                            BufferRam[indexSave].bmePressure,
                            BufferRam[indexSave].bmeTemp);
      
      fileDust.writeToFile(
                            BufferRam[indexSave].ramTime,
                            BufferRam[indexSave].dustPM1_0,
                            BufferRam[indexSave].dustPM2_5,
                            BufferRam[indexSave].dustPM10,
                            BufferRam[indexSave].dustOK,
                            BufferRam[indexSave].dustError);

      // INCREMENT COUNTER
      indexSave++;

    } // END while
    
    // REST WRITING COUNTERS
    counter_ram = 0;
  } // END if

} // END loop() --------------------------------------------------------

/***********************************************************************
 * FUNCTION DEFINITIONS
 **********************************************************************/