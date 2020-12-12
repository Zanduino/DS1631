/*!
  @file DS1631.cpp
  @section DS1631cpp_intro_section Description
  Arduino Library for Microchip SRAM access\n\n
  See main library header file for details
*/
#include "DS1631.h"               // Include the header definition
DS1631_Class::DS1631_Class() {}   ///< Unused class constructor
DS1631_Class::~DS1631_Class() {}  ///< Unused class destructor

uint8_t DS1631_Class::begin(const uint32_t i2cSpeed) {
  /*!
    @brief     Starts I2C communications and search all possible address for up to 8 different
               DS1631 devices
    @details   Each device found is stored in the _Devices array
    @param[in] i2cSpeed I2C Communications speed. If not specified defaults to
               I2C_STANDARD_MODE (100KHz)
    @return    Number of devices found
  */
  Wire.begin();             // Start I2C as master device
  Wire.setClock(i2cSpeed);  // Set the I2C bus speed
  for (uint8_t i = DS1631_MIN_ADDRESS; i < (DS1631_MIN_ADDRESS + DS1631_MAX_DEVICES);
       i++)  // All addresses
  {
    Wire.beginTransmission(i);  // Check address
    if (Wire.endTransmission() == 0) {
      writeCommand(i, DS1631_STOP_CONVERT);  // Stop any active conversion
      writeByte(i, DS1631_ACCESS_CONFIG,
                readByte(i, DS1631_ACCESS_CONFIG) & 0xFE);  // unset bit 0 to start continuous
      writeCommand(i, DS1631_START_CONVERT);                // Begin conversions
      _Devices[_thermometers++] = i;                        // Add address to list
    }                                                       // of if-then device detected
  }                              // of for-next each possible device address
  thermometers = _thermometers;  // Copy value to public variable
  return _thermometers;          // Return number of DS1631 devices found
}  // of method begin()
void DS1631_Class::writeCommand(const uint8_t device, const uint8_t command) const {
  /*!
    @brief     writes a command to a I2C address (without any data)
    @param[in] device I2C address of device
    @param[in] command Single-Byte command to write
  */
  Wire.beginTransmission(device);  // Address the I2C device
  Wire.write(command);             // Send the command to DS1631
  Wire.endTransmission();          // Close transmission
}  // of method writeCommand()
uint8_t DS1631_Class::readByte(const uint8_t device, const uint8_t command) const {
  /*!
    @brief     reads a single byte from a device and address
    @param[in] device I2C address of device
    @param[in] command Single-Byte address to read from
    @return    Single byte read
  */
  Wire.beginTransmission(device);        // Address the I2C device
  Wire.write(command);                   // Send register address to read
  Wire.endTransmission();                // Close transmission
  Wire.requestFrom(device, (uint8_t)1);  // Request 1 byte of data
  return (Wire.read());                  // Return the byte that we've read
}  // of method readByte()
uint16_t DS1631_Class::readWord(const uint8_t device, const uint8_t command) const {
  /*!
    @brief     reads a word (2 bytes) from a device and address
    @param[in] device I2C address of device
    @param[in] command Single-Byte address to read from
    @return    2 bytes read
  */
  Wire.beginTransmission(device);          // Address the I2C device
  Wire.write(command);                     // Send register address to read
  Wire.endTransmission();                  // Close transmission
  Wire.requestFrom(device, (uint8_t)2);    // Request 2 bytes of data
  uint16_t returnData = Wire.read() << 8;  // Read MSB
  returnData |= Wire.read();               // Read LSB
  return (returnData);                     // Return the bytes that we've read
}  // of method readByte()
uint8_t DS1631_Class::writeByte(const uint8_t device, const uint8_t command,
                                const uint8_t data) const {
  /*!
    @brief     writes a single byte to a device and address
    @param[in] device I2C address of device
    @param[in] command Single-Byte address to write to
    @param[in] data Byte to write
    @return    Transmission status, 0 on success otherwise I2C Error Code
  */
  Wire.beginTransmission(device);   // Address the I2C device
  Wire.write(command);              // Send register address to read
  Wire.write(data);                 // Send the data
  return (Wire.endTransmission());  // Close transmission and return the status
}  // of method writeByte()
uint8_t DS1631_Class::writeWord(const uint8_t device, const uint8_t command,
                                const int16_t data) const {
  /*!
   @brief     writes a word (2 bytes) to a device and address
   @param[in] device I2C address of device
   @param[in] command Single-Byte address to write to
   @param[in] data Word to write
   @return    Transmission status, 0 on success otherwise I2C Error Code
  */
  Wire.beginTransmission(device);    // Address the I2C device
  Wire.write(command);               // Send register address to read
  Wire.write((uint8_t)(data >> 8));  // Send the MSB first
  Wire.write((uint8_t)data);         // then the LSB
  return (Wire.endTransmission());   // Close transmission and return the status
}  // of method writeWord()
int16_t DS1631_Class::readTemp(const uint8_t device) const {
  /*!
    @brief    return the current temperature value for a given device number
    @details  If no device is specified then device[0] is used. The DS1631 returns raw values in
              0.0625°C increments. A check is done to see if there is still an active conversion
              being processed and will wait until any conversion is complete.
    @param[in] device Device number
    @return    Temperature in units of 0.0625 degrees Celsius
  */
  int16_t returnTemperature = 0;
  if (device >= _thermometers)
    return INT16_MIN;  // Return error value if the device is out of bounds
  if (readByte(_Devices[device], DS1631_ACCESS_CONFIG) &
      1)  // If we are in one-shot mode then wait for conversion
  {
    while (!(readByte(_Devices[device], DS1631_ACCESS_CONFIG) & 0x80)) {
    }  // Wait...
  }    // if-then conversion is active
  returnTemperature = readWord(_Devices[device], DS1631_READ_TEMPERATURE) >> 4;  // Read values
  if (returnTemperature & 0x800) returnTemperature |= 0xF000;  // Keep negative sign
  return returnTemperature;
}  // of method readTemp()
void DS1631_Class::setPrecision(const uint8_t device, const uint8_t precision) const {
  /*!
    @brief    Set the precision of the DS1631 to 9, 10, 11 or 12 bits.
    @details  This corresponds to increments of 0.5°C, 0.25°C, 0.125°C and 0.0625°C and conversion
              times of 94ms, 188ms, 375ms and 750ms respectively.
    @param[in] device Device number
    @param[in] precision 9, 10, 11 or 12 for number of bits precision to use. Invalid values are
    ignored
  */
  if (precision > 8 && precision < 13 && device < _thermometers)  // If the precision is in range
  {
    writeByte(_Devices[device], DS1631_ACCESS_CONFIG,
              (readByte(_Devices[device], DS1631_ACCESS_CONFIG) & 0xF3) | ((precision - 9) << 2));
  }  // of if-then precision is in range
}  // of method setPrecision()
void DS1631_Class::setAlarmTemperature(const uint8_t device, const uint8_t alarmType,
                                       int16_t alarmTemp) const {
  /*!
    @brief     set the 16 bit value for the temperature alarm on a device and also reset the given
               alarm
    @details   A 0 in "alarmType" equates to low, any other value to high alarm. The alarmTemp is
               specified in internal units, that is in increments of 0.0625°C
    @param[in] device Device number
    @param[in] alarmType Alarm type to set. 0 means "low" and any other value means "high"
    @param[in] alarmTemp Alarm temperature in units of 0.0625°C
  */
  alarmTemp = alarmTemp << 4;  // Shift over 4 bits
  if (device < _thermometers)  // Skip if device is out of bounds
  {
    if (alarmType == 0)  // Branch here if setting low alarm
    {
      writeWord(_Devices[device], DS1631_ACCESS_TL, alarmTemp);  // Set the low temperature
      // Reset the indicator flag
      writeByte(_Devices[device], DS1631_ACCESS_CONFIG,
                readByte(_Devices[device], DS1631_ACCESS_CONFIG) & ~0x20);
    } else  // Branch here if setting high alarm
    {
      writeWord(_Devices[device], DS1631_ACCESS_TH, alarmTemp);  // Set the high temperature
      // Reset the indicator flag
      writeByte(_Devices[device], DS1631_ACCESS_CONFIG,
                readByte(_Devices[device], DS1631_ACCESS_CONFIG) & ~0x40);
    }  // of if-then-else set low or high temperature alarm
  }    // of if-then invalid device number
}  // of method setAlarmTemperature()
uint8_t DS1631_Class::getAlarm(const uint8_t device) const {
  /*!
    @brief     Check to see if any alarms are active on a device
    @details   When an out-of-range device number is specified the function returns a UINT8_MAX
    value
    @param[in] device Device number
    @return    1 when the device has an active LOW alarm, 2 when the HIGH alarm is active, otherwise
               returns 0
  */
  if (device >= _thermometers) return UINT8_MAX;  // Error if device index out of bounds
  uint8_t controlBuffer =
      readByte(_Devices[device], DS1631_ACCESS_CONFIG);  // Get configuration register
  writeByte(_Devices[device], DS1631_ACCESS_CONFIG, controlBuffer & 0x9F);  // Clear 2 alarm bits
  return (controlBuffer >> 5) & 3;  // Return shifted over 2 alarm bits
}  // of method getAlarm()
void DS1631_Class::setPolarity(const uint8_t device, const bool polarity) const {
  /*!
    @brief    set the polarity output pin to assert when high or low
    @details   When an out-of-range device number is specified the function returns a UINT8_MAX
    value
    @param[in] device Device number
    @param[in] polarity Polarity to set. true means HIGH and false means LOW
  */
  if (device >= _thermometers) return;
  // update buffer with boolean value of polarity setting
  writeByte(_Devices[device], DS1631_ACCESS_CONFIG,
            (readByte(_Devices[device], DS1631_ACCESS_CONFIG) & 0xFD) & (polarity << 1));
}  // of method setPolarity()
void DS1631_Class::setContinuous(const uint8_t device, const bool continuous) const {
  /*!
    @brief     set the DS1631 to continuous measurements
    @details   If the optional "continuous" boolean parameter is set to false, it will turn off
               continuous and activate one-shot measurements
    @param[in] device Device number
    @param[in] continuous Defaults to "true" if not specified, if set to false then one-shot
               measurements are done
  */
  if (device >= _thermometers) return;
  // update buffer with boolean value of continuous switch
  writeByte(_Devices[device], DS1631_ACCESS_CONFIG,
            (readByte(_Devices[device], DS1631_ACCESS_CONFIG) & 0xFE) & ~continuous);
}  // of method setContinuous()
void DS1631_Class::reset(const uint8_t device) const {
  /*!
    @brief     Performs a device reset
    @param[in] device Device number
  */
  if (device < _thermometers) writeCommand(device, DS1631_POR);  // Send power-on-reset command
}  // of method reset()
