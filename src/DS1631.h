/*!
  @file DS1631.h

  @mainpage Arduino Library Header to access a Microchip SRAM

  @section DS1631_intro_section Description

 Class definition header for the DS1631 I2C thermometer from Maxim integrated, described at
 https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS1631.html. The
DS1631's data sheet is located at https://datasheets.maximintegrated.com/en/ds/DS1631-DS1731.pdf.
\n\n

 The most recent version of this library can be found at https://github.com/Zanduino/DS1631 and a
detailed library description is on the library's wiki at https://github.com/Zanduino/DS1631/wiki
\n\n

 The DS1631 is accurate to within �0.5�C Accuracy over 0�C to +70�C Range and has a user-selectable
resolution from 9 to 12 bits. The DS1631 can also be used as a thermostat with a dedicated output
pin. The address is user-selectable and up to 8 devices can be put on a single I2C bus

@section doxygen doxygen configuration

This library is built with the standard "Doxyfile", which is located at
https://github.com/Zanduino/Common/blob/main/Doxygen. As described on that page, there are only 5
environment variables used, and these are set in the project's actions file, located at
https://github.com/Zanduino/BME280/blob/master/.github/workflows/ci-doxygen.yml
Edit this file and set the 5 variables -  PRETTYNAME, PROJECT_NAME, PROJECT_NUMBER, PROJECT_BRIEF
and PROJECT_LOGO so that these values are used in the doxygen documentation.
The local copy of the doxyfile should be in the project's root directory in order to do local
doxygen testing, but the file is ignored on upload to GitHub.

@section clang-format
Part of the GitHub actions for CI is running every source file through "clang-format" to ensure
that coding formatting is done the same for all files. The configuration file ".clang-format" is
located at https://github.com/Zanduino/Common/tree/main/clang-format and this is used for CI tests
when pushing to GitHub. The local file, if present in the root directory, is ignored when
committing and uploading.

@section license GNU General Public License v3.0

This program is free software: you can redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version. This program is distributed in the hope that it
will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details. You should
have received a copy of the GNU General Public License along with this program.  If not, see
<http://www.gnu.org/licenses/>.

@section author Author

Written by Arnd <Zanshin_Github@sv-zanshin.com> / https://www.github.com/SV-Zanshin

 @section versions Changelog

 Version| Date       | Developer  | Comments
 ------ | ---------- | ---------- | --------
 1.0.4  | 2019-01-26 | SV-Zanshin | Issue #9 - converted documentation to doxygen
 1.0.3  | 2019-01-27 | SV-Zanshin | Fixed overflow on I2C data types
 1.0.2  | 2018-07-02 | SV-Zanshin | Added guard code against multiple I2C Speed definitions
 1.0.2  | 2018-06-30 | SV-Zanshin | Issue #3 to add option to set I2C speed
 1.0.1  | 2018-06-24 | SV-Zanshin | Minor code cleanup and refactoring
 1.0.1  | 2018-06-24 | SV-Zanshin | Minor code cleanup and refactoring
 1.0.0  | 2017-08-23 | SV-Zanshin | Initial release
 1.0.0b | 2017-08-20 | SV-Zanshin | Initial coding
*/

#include <Wire.h>  // Standard I2C "Wire" library

#include "Arduino.h"  // Arduino data type definitions
#ifndef DS1631_h
/*! Guard code definition to prevent multiple definitions */
#define DS1631_h
/***************************************************************************************************
** Declare constants used in the class                                                            **
***************************************************************************************************/
#ifndef I2C_MODES
/*! Guard code definition to prevent multiple definitions */
#define I2C_MODES
const uint32_t I2C_STANDARD_MODE{100000};  ///< Default normal I2C 100KHz speed
const uint32_t I2C_FAST_MODE{400000};      ///< Fast mode
#endif
const uint8_t DS1631_MAX_DEVICES{8};          ///< Maximum number of DS1631 devices
const uint8_t DS1631_MIN_ADDRESS{0x48};       ///< Minimum DS1631 address
const uint8_t DS1631_ACCESS_TH{0xA1};         ///< Access Temperature High bytes
const uint8_t DS1631_ACCESS_TL{0xA2};         ///< Access Temperature Low bytes
const uint8_t DS1631_READ_TEMPERATURE{0xAA};  ///< Read last converted temperature
const uint8_t DS1631_ACCESS_CONFIG{0xAC};     ///< Access Configuration Byte
const uint8_t DS1631_START_CONVERT{0x51};     ///< Start a temperature conversion
const uint8_t DS1631_STOP_CONVERT{0x22};      ///< Stop any conversions
const uint8_t DS1631_POR{0x54};               ///< Simulate Power-On-Reset

class DS1631_Class {
  /*!
    @class   DS1631_Class
    @brief   Forward class declaration for the DS1631 I2C Thermometer class
  */
 public:
  uint8_t thermometers;  ///< Number of thermometers found
  DS1631_Class();
  ~DS1631_Class();
  uint8_t begin(const uint32_t i2cSpeed = I2C_STANDARD_MODE);
  int16_t readTemp(const uint8_t device = 0) const;
  void    setPrecision(const uint8_t device, const uint8_t precision) const;
  void setAlarmTemperature(const uint8_t device, const uint8_t alarmType, int16_t alarmTemp) const;
  uint8_t getAlarm(const uint8_t device) const;
  void    setPolarity(const uint8_t device, const bool polarity) const;
  void    setContinuous(const uint8_t device, const bool continuous = true) const;
  void    reset(const uint8_t device) const;

 private:
  uint8_t  _thermometers;                       ///< Number of thermometers found
  uint8_t  _Devices[DS1631_MAX_DEVICES] = {0};  ///< List of device addresses
  void     writeCommand(const uint8_t device, const uint8_t command) const;
  uint8_t  readByte(const uint8_t device, const uint8_t command) const;
  uint16_t readWord(const uint8_t device, const uint8_t command) const;
  uint8_t  writeByte(const uint8_t device, const uint8_t command, const uint8_t data) const;
  uint8_t  writeWord(const uint8_t device, const uint8_t command, const int16_t data) const;
};  // of DS1631 class definition
#endif
