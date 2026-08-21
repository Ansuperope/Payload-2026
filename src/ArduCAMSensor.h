
// /**
//  * @file ArduCAMSensor.h
//  * @brief Interface for ArduCAM camera module
//  *
//  * Handles initialization and image capture for
//  * an ArduCAM Mini camera.
//  */

// #ifndef ARDUCAM_SENSOR_H
// #define ARDUCAM_SENSOR_H

// #include <Arduino.h>
// #include <SPI.h>
// #include <Wire.h>
// #include <ArduCAM.h>

// /**
//  * @struct CameraPacket
//  * @brief Stores the result of an ArduCAM image capture
//  *
//  * The image itself is stored in the ArduCAM FIFO.
//  * This packet only stores information about the
//  * capture status and image size.
//  */
// struct CameraPacket {

//   /** True if an image was successfully captured */
//   bool cameraOK;

//   /**
//    * Error code:
//    * 0 = no error
//    * 1 = camera initialization/capture failure
//    * 2 = timeout waiting for capture
//    */
//   uint8_t errorCode;

//   /** Number of bytes stored in the camera FIFO */
//   uint32_t imageSize;
// };

// /**
//  * @class ArduCAMSensor
//  * @brief Class for interacting with an ArduCAM camera
//  */
// class ArduCAMSensor {
// public:

//   /**
//    * @brief Create an ArduCAM sensor object
//    * @param chipSelectPin SPI chip-select pin
//    */
//   explicit ArduCAMSensor(uint8_t chipSelectPin);

//   /**
//    * @brief Initialize the ArduCAM
//    * @return true if initialization succeeds
//    * @return false if initialization fails
//    */
//   bool begin();

//   /**
//    * @brief Capture an image
//    * @return CameraPacket containing capture status
//    */
//   CameraPacket read();

//   /**
//    * @brief Save the most recently captured image to an SD file
//    * @param filename Name of the image file
//    * @return true if the image was saved successfully
//    */
//   bool saveImage(const char *filename);

// private:

//   /** ArduCAM camera object */
//   ArduCAM camera;

//   /** SPI chip-select pin */
//   uint8_t csPin;

//   /** Check whether the SPI interface is working */
//   bool checkSPI();

//   /** Wait for the camera to finish capturing */
//   bool waitForCapture();
// };

// #endif