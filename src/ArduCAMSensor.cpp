
// /**
//  * @file ArduCAMSensor.cpp
//  * @brief Implementation of ArduCAM camera module
//  */

// #include "ArduCAMSensor.h"
// #include <SD.h>

// /**
//  * @brief Create an ArduCAM sensor object
//  */
// ArduCAMSensor::ArduCAMSensor(uint8_t chipSelectPin)
//     : camera(OV2640, chipSelectPin),
//       csPin(chipSelectPin) {
// }

// /**
//  * @brief Initialize the ArduCAM
//  */
// bool ArduCAMSensor::begin() {

//   /**
//    * Start SPI communication.
//    */
//   SPI.begin();

//   /**
//    * Configure the camera chip-select pin.
//    */
//   pinMode(csPin, OUTPUT);
//   digitalWrite(csPin, HIGH);

//   /**
//    * Initialize the ArduCAM SPI interface.
//    */
//   camera.write_reg(ARDUCHIP_TEST1, 0x55);

//   /**
//    * Verify that the SPI interface is responding.
//    */
//   if (camera.read_reg(ARDUCHIP_TEST1) != 0x55) {
//     return false;
//   }

//   /**
//    * Initialize the camera module.
//    */
//   camera.set_format(JPEG);

//   delay(100);

//   /**
//    * Initialize the OV2640.
//    */
//   camera.InitCAM();

//   /**
//    * Set JPEG resolution.
//    *
//    * FRAMESIZE_QVGA = 320 x 240
//    *
//    * This is intentionally kept relatively small because
//    * the image will eventually be stored on the SD card.
//    */
//   camera.OV2640_set_JPEG_size(OV2640_320x240);

//   delay(100);

//   /**
//    * Clear any previous image data from the FIFO.
//    */
//   camera.clear_fifo_flag();

//   return true;
// }

// /**
//  * @brief Check whether the ArduCAM SPI interface works
//  */
// bool ArduCAMSensor::checkSPI() {

//   camera.write_reg(ARDUCHIP_TEST1, 0x55);

//   return camera.read_reg(ARDUCHIP_TEST1) == 0x55;
// }

// /**
//  * @brief Wait for the camera to finish capturing
//  */
// bool ArduCAMSensor::waitForCapture() {

//   /**
//    * Wait up to 2 seconds for the capture to complete.
//    */
//   const uint32_t timeout = 2000;
//   uint32_t startTime = millis();

//   while (!(camera.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))) {

//     if (millis() - startTime >= timeout) {
//       return false;
//     }

//     delay(1);
//   }

//   return true;
// }

// /**
//  * @brief Capture an image
//  */
// CameraPacket ArduCAMSensor::read() {

//   CameraPacket data;

//   // Default values
//   data.cameraOK = false;
//   data.errorCode = 0;
//   data.imageSize = 0;

//   /**
//    * Verify SPI communication before attempting capture.
//    */
//   if (!checkSPI()) {
//     data.errorCode = 1;
//     return data;
//   }

//   /**
//    * Clear the FIFO before taking a new image.
//    */
//   camera.clear_fifo_flag();

//   /**
//    * Start image capture.
//    */
//   camera.start_capture();

//   /**
//    * Wait for capture to finish.
//    */
//   if (!waitForCapture()) {
//     data.errorCode = 2;
//     return data;
//   }

//   /**
//    * Get the number of bytes in the FIFO.
//    */
//   data.imageSize = camera.read_fifo_length();

//   /**
//    * Make sure the image isn't empty.
//    */
//   if (data.imageSize == 0) {
//     data.errorCode = 1;
//     return data;
//   }

//   /**
//    * The image was successfully captured.
//    */
//   data.cameraOK = true;
//   data.errorCode = 0;

//   return data;
// }

// /**
//  * @brief Save the most recently captured image to an SD card
//  */
// bool ArduCAMSensor::saveImage(const char *filename) {

//   /**
//    * Get the image size from the camera FIFO.
//    */
//   uint32_t length = camera.read_fifo_length();

//   if (length == 0) {
//     return false;
//   }

//   /**
//    * Open the SD file.
//    */
//   File imageFile = SD.open(filename, FILE_WRITE);

//   if (!imageFile) {
//     return false;
//   }

//   /**
//    * Read image data from the camera FIFO in chunks.
//    *
//    * Reading in chunks prevents us from needing to
//    * allocate the entire image in RAM.
//    */
//   const uint16_t BUFFER_SIZE = 256;
//   uint8_t buffer[BUFFER_SIZE];

//   uint32_t remaining = length;

//   while (remaining > 0) {

//     uint16_t bytesToRead =
//         (remaining > BUFFER_SIZE)
//             ? BUFFER_SIZE
//             : remaining;

//     for (uint16_t i = 0; i < bytesToRead; i++) {
//       buffer[i] = camera.read_fifo();
//     }

//     imageFile.write(buffer, bytesToRead);

//     remaining -= bytesToRead;
//   }

//   imageFile.close();

//   /**
//    * Clear the FIFO after saving the image.
//    */
//   camera.clear_fifo_flag();

//   return true;
// }