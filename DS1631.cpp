/*******************************************************************************************************************
** DS1631 class method definitions. See the DS1631.h header file for program details and version information      **
**                                                                                                                **
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU General     **
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your      **
** option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY     **
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   **
** GNU General Public License for more details. You should have received a copy of the GNU General Public License **
** along with this program.  If not, see <http://www.gnu.org/licenses/>.                                          **
**                                                                                                                **
*******************************************************************************************************************/
#include "DS1631.h"                                                           // Include the header definition    //
                                                                              //                                  //
DS1631_Class::DS1631_Class()  {}                                              // Unused class constructor         //
DS1631_Class::~DS1631_Class() {}                                              // Unused class destructor          //
/*******************************************************************************************************************
** Method begin starts I2C communications and search all possible address for up to 8 different DS1631 devices.   **
** Return the number of devices found and also set the global variable                                            **
*******************************************************************************************************************/
uint8_t DS1631_Class::begin() {                                               // Start I2C communications         //
  Wire.begin();                                                               // Start I2C as master device       //
  for (uint8_t i=DS1631_MIN_ADDRESS;i<(DS1631_MIN_ADDRESS+8);i++) {           // Loop through possible addresses  //
    Wire.beginTransmission(i);                                                // Check address                    //
    if (Wire.endTransmission()==0) {                                          // If there's a device present      //
      writeCommand(i,DS1631_STOP_CONVERT);                                    // Stop any active conversion       //
      writeByte(i,DS1631_ACCESS_CONFIG,readByte(i,DS1631_ACCESS_CONFIG)&0xFE);// unset bit 0 to start continuous  //
      writeCommand(i,DS1631_START_CONVERT);                                   // Begin conversions                //
      _Devices[thermometers++] = i;                                           // Add address to list              //
    } // of if-then device detected                                           //                                  //
  } // of for-next each possible device address                               //                                  //
  return thermometers;                                                        // Return number of DS1631 devices  //
} // of method begin()                                                        //                                  //
/*******************************************************************************************************************
** Method writeCommand() writes a command to a I2C address without any data                                       **
*******************************************************************************************************************/
void DS1631_Class::writeCommand(const uint8_t device, const uint8_t command){ //                                  //
  Wire.beginTransmission(device);                                             // Address the I2C device           //
  Wire.write(command);                                                        // Send the command to DS1631       //
  _TransmissionStatus = Wire.endTransmission();                               // Close transmission               //
} // of method writeCommand()                                                 //                                  //
/*******************************************************************************************************************
** Method readByte() reads a single byte from a device and address                                                **
*******************************************************************************************************************/
uint8_t DS1631_Class::readByte(const uint8_t device, const uint8_t command) { // Read a byte from DS1631          //
  Wire.beginTransmission(device);                                             // Address the I2C device           //
  Wire.write(command);                                                        // Send register address to read    //
  _TransmissionStatus = Wire.endTransmission();                               // Close transmission               //
  Wire.requestFrom(device, (uint8_t)1);                                       // Request 1 byte of data           //
  return(Wire.read());                                                        // Return the byte that we've read  //
} // of method readByte()                                                     //                                  //
/*******************************************************************************************************************
** Method readWord() reads a word (2 bytes) from a device and address                                             **
*******************************************************************************************************************/
uint16_t DS1631_Class::readWord(const uint8_t device, const uint8_t command) {// Read a word from DS1631          //
  Wire.beginTransmission(device);                                             // Address the I2C device           //
  Wire.write(command);                                                        // Send register address to read    //
  _TransmissionStatus = Wire.endTransmission();                               // Close transmission               //
  Wire.requestFrom(device, (uint8_t)2);                                       // Request 2 bytes of data          //
  uint16_t returnData = Wire.read() << 8;                                     // Read MSB                         //
  returnData |= Wire.read();                                                  // Read LSB                         //
  return(returnData);                                                         // Return the bytes that we've read //
} // of method readByte()                                                     //                                  //
/*******************************************************************************************************************
** Method writeByte() writes a byte to a given device and address                                                 **
*******************************************************************************************************************/
uint8_t DS1631_Class::writeByte(const uint8_t device, const uint8_t command,  // write a byte from DS1631         //
                                const uint8_t data ) {                        //                                  //
  Wire.beginTransmission(device);                                             // Address the I2C device           //
  Wire.write(command);                                                        // Send register address to read    //
  Wire.write(data);                                                           // Send the data                    //
  _TransmissionStatus = Wire.endTransmission();                               // Close transmission               //
  return(_TransmissionStatus);                                                // Return the status                //
} // of method writeByte()                                                    //                                  //
/*******************************************************************************************************************
** Method writeWord() writes a word (2 bytes) to a given device and address                                       **
*******************************************************************************************************************/
uint8_t DS1631_Class::writeWord(const uint8_t device, const uint8_t command,  // write a byte from DS1631         //
                                const int16_t data ) {                        //                                  //
  Wire.beginTransmission(device);                                             // Address the I2C device           //
  Wire.write(command);                                                        // Send register address to read    //
  Wire.write((uint8_t)(data>>8));                                             // Send the MSB first               //
  Wire.write((uint8_t)data);                                                  // then the LSB                     //
  _TransmissionStatus = Wire.endTransmission();                               // Close transmission               //
  return(_TransmissionStatus);                                                // Return the status                //
} // of method writeWord()                                                    //                                  //
/*******************************************************************************************************************
** method readTemp() to return the current temperature value for a given device number, if no device is specified **
** then the default 0 is used. The DS1631 returns raw values in 0.0625°C increments. A check is done to see if    **
** is still an active conversion being process and will wait until any conversion is complete.                    **
*******************************************************************************************************************/
int16_t DS1631_Class::readTemp(const uint8_t device) {                        // Read the device temperature      //
  int16_t returnTemperature = 0;                                              //                                  //
  if (device>=thermometers) return INT16_MIN;                                 // Error if device out of bounds    //
  if (readByte(_Devices[device],DS1631_ACCESS_CONFIG)&1)                      // If we are in one-shot mode then  //
    while(!(readByte(_Devices[device],DS1631_ACCESS_CONFIG)&0x80)) {}         // Wait active conversion completed //
  returnTemperature = readWord(_Devices[device],DS1631_READ_TEMPERATURE)>>4;  // Read values                      //
  if (returnTemperature&0x800) returnTemperature |= 0xF000;                   // Keep negative sign               //
  return returnTemperature;                                                   // Return in 0.0625 degrees C       //
} // of method readTemp()                                                     //                                  //
/*******************************************************************************************************************
** method setPrecision() will set the precision of the DS1631 to 9, 10, 11 or 12 bits. This corresponds to        **
** increments of 0.5°C, 0.25°C, 0.125°C and 0.0625°C and conversion times of 94mx, 188ms, 375ms and 750ms         **
** respectively                                                                                                   **
*******************************************************************************************************************/
void DS1631_Class::setPrecision(const uint8_t device,const uint8_t precision){// Set the device precision         //
  if(precision>8 && precision<13 && device<thermometers) {                    // If the precision is in range     //
    writeByte(_Devices[device],DS1631_ACCESS_CONFIG,                          // Update the 2 precision bits with //
    (readByte(_Devices[device],DS1631_ACCESS_CONFIG)&0xF3)|((precision-9)<<2));//the new value                    //
  } // of if-then precision is in range                                       //                                  //
} // of method setPrecision()                                                 //                                  //
/*******************************************************************************************************************
** method setAlarmTemperature() will set the 16 bit value for the temperature alarm on a device and also reset    **
** the appropriate alarm. A 0 in "alarmType" equates to low, any other value to high alarm. The alarmTemp is      **
** specified in internal units, that is in increments of 0.0625°C                                                 **
*******************************************************************************************************************/
void DS1631_Class::setAlarmTemperature(const uint8_t device,                  // Set low or high alarm temperature//
                                       const uint8_t alarmType,               // on the specified device          //
                                             int16_t alarmTemp) {             //                                  //
  alarmTemp = alarmTemp<<4;                                                   // Shift over 4 bits                //
  if (device<thermometers) {                                                  // Skip if device is out of bounds  //
    if (alarmType==0) {                                                       // Branch here if setting low alarm //
      writeWord(_Devices[device],DS1631_ACCESS_TL,alarmTemp);                 // Set the low temperature          //
      writeByte(_Devices[device],DS1631_ACCESS_CONFIG,                        // Reset the indicator flag         //
      readByte(_Devices[device],DS1631_ACCESS_CONFIG)&~0x20);                 // to read 0                        //
    } else {                                                                  //                                  //
      writeWord(_Devices[device],DS1631_ACCESS_TH,alarmTemp);                 // Set the high temperature         //
      writeByte(_Devices[device],DS1631_ACCESS_CONFIG,                        // Reset the indicator flag         //
      readByte(_Devices[device],DS1631_ACCESS_CONFIG)&~0x40);                 // to read 0                        //
    } // of if-then-else set low or high temperature alarm                    //                                  //
  } // of if-then invalid device number                                       //                                  //
} // of method setAlarmTemperature()                                          //                                  //
/*******************************************************************************************************************
** method getAlarm() will return 0 when no alarms are set, 1 when low flag is set, 2 when high flag is set or 3   **
** when both alarms are set. An out-of-bounds device will return high-value. Any active alarms are cleared by     **
** calling this function                                                                                          **
*******************************************************************************************************************/
uint8_t DS1631_Class::getAlarm(const uint8_t device) {                        // Read the device temperature      //
  if (device>=thermometers) return UINT8_MAX;                                 // Error if device out of bounds    //
  uint8_t controlBuffer = readByte(_Devices[device],DS1631_ACCESS_CONFIG);    // Get configuration register       //
  writeByte(_Devices[device],DS1631_ACCESS_CONFIG,controlBuffer&0x9F);        // Clear 2 alarm bits               //
  return (controlBuffer>>5)&3;                                                // Return shifted over 2 alarm bits //
} // of method getAlarm()                                                     //                                  //
/*******************************************************************************************************************
** method setPolarity() will set the polarity output pin to assert when high or low                               **
*******************************************************************************************************************/
void DS1631_Class::setPolarity(const uint8_t device, const bool polarity) {   // Set polarity pin to high or low  //
  writeByte(_Devices[device],DS1631_ACCESS_CONFIG,                            // update buffer with boolean value //
  (readByte(_Devices[device],DS1631_ACCESS_CONFIG)&0xFD)&(polarity<<1));      // of polarity setting              //
} // of method setPolarity()                                                  //                                  //
/*******************************************************************************************************************
** method setContinuous() will set the DS1631 to continuous measurements unless the optional "continuous" boolean **
** parameter is set to false, in which case one-shot measurements are activated                                   **
*******************************************************************************************************************/
void DS1631_Class::setContinuous(const uint8_t device, const bool continuous){// Set to continuous or one-shot    //
  writeByte(_Devices[device],DS1631_ACCESS_CONFIG,                            // update buffer with boolean value //
  (readByte(_Devices[device],DS1631_ACCESS_CONFIG)&0xFE)&~continuous);        // of continuous switch             //
} // of method setContinuous()                                                //                                  //
