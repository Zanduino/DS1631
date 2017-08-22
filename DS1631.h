/*******************************************************************************************************************
** Class definition header for the DS1631 I2C thermometer from Maxim integrated, described at                     **
** https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS1631.html. The DS1631's data **
** sheet is located at https://datasheets.maximintegrated.com/en/ds/DS1631-DS1731.pdf.                            **
**                                                                                                                **
** The most recent version of this library can be found at https://github.com/SV-Zanshin/DS1631 and a detailed    **
** library description is on the library's wiki at https://github.com/SV-Zanshin/DS1631/wiki                      **
**                                                                                                                **
** The DS1631 is accurate to within ±0.5°C Accuracy over 0°C to +70°C Range and has a user-selectable resolution  **
** from 9 to 12 bits. The DS1631 can also be used as a thermostat with a dedicated output pin. The address is     **
** user-selectable and up to 8 devices can be put on a single I2C bus                                             **
**                                                                                                                **
** Although programming for the Arduino and in c/c++ is new to me, I'm a professional programmer and have learned,**
** over the years, that it is much easier to ignore superfluous comments than it is to decipher non-existent ones;**
** so both my comments and variable names tend to be verbose. The code is written to fit in the first 80 spaces   **
** and the comments start after that and go to column 117 - allowing the code to be printed in A4 landscape mode. **
**                                                                                                                **
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU General     **
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your      **
** option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY     **
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   **
** GNU General Public License for more details. You should have received a copy of the GNU General Public License **
** along with this program.  If not, see <http://www.gnu.org/licenses/>.                                          **
**                                                                                                                **
** Vers.  Date       Developer           Comments                                                                 **
** ====== ========== =================== ======================================================================== **
** 1.0.0b 2017-08-20 Arnd@SV-Zanshin.Com Initial coding                                                           **
**                                                                                                                **
*******************************************************************************************************************/
#include "Arduino.h"                                                          // Arduino data type definitions    //
#include <Wire.h>                                                             // Standard I2C "Wire" library      //
#ifndef DS1631_h                                                              // Guard code definition            //
  #define DS1631_h                                                            // Define the name inside guard code//
  /*****************************************************************************************************************
  ** Declare constants used in the class                                                                          **
  *****************************************************************************************************************/
  const uint8_t DS1631_MAX_DEVICES               =    8;                      // Maximum number of DS1631 devices //
  const uint8_t DS1631_MIN_ADDRESS               = 0x48;                      // Minimum DS1631 address           //
  const uint8_t DS1631_ACCESS_TH                 = 0xA1;                      // Access Temperature High bytes    //
  const uint8_t DS1631_ACCESS_TL                 = 0xA2;                      // Access Temperature Low bytes     //
  const uint8_t DS1631_READ_TEMPERATURE          = 0xAA;                      // Read last converted temperature  //
  const uint8_t DS1631_ACCESS_CONFIG             = 0xAC;                      // Access Configuration Byte        //
  const uint8_t DS1631_START_CONVERT             = 0x51;                      // Start a temperature conversion   //
  const uint8_t DS1631_STOP_CONVERT              = 0x22;                      // Stop any conversions             //
  /*****************************************************************************************************************
  ** Main DS1631 class for the I2C thermometers                                                                   **
  *****************************************************************************************************************/
  class DS1631_Class {                                                        // Class definition                 //
    public:                                                                   // Publicly visible methods         //
      DS1631_Class();                                                         // Class constructor                //
      ~DS1631_Class();                                                        // Class destructor                 //
      uint8_t begin();                                                        // Start & find all DS1631 devices  //
      uint8_t thermometers;                                                   // Number of thermometers found     //
      int16_t readTemp(const uint8_t device=0);                               // Read the device temperature      //
      void    setPrecision(const uint8_t device,const uint8_t precision);     // Set the device precision         //
      void    setAlarmTemperature(const uint8_t device,                       // Set low or high alarm temperature//
                                  const uint8_t alarmType,                    // on the specified device          //
                                        int16_t alarmTemp);                   //                                  //
      uint8_t getAlarm(const uint8_t device);                                 // Return alarm state of device     //
      void    setPolarity(const uint8_t device, const bool polarity);         // Set polarity pin to high or low  //
      void    setContinuous(const uint8_t device,const bool continuous=true); // Set to continuous or one-shot    //
    private:                                                                  // Private methods                  //
      uint8_t _Devices[DS1631_MAX_DEVICES]={0};                               // List of device addresses         //
      uint8_t _TransmissionStatus = 0;                                        // I2C read/write error code        //
      void     writeCommand(const uint8_t device, const uint8_t command);     // Write a command to a device      //
      uint8_t  readByte(const uint8_t device, const uint8_t command);         // Read a byte from device/address  //
      uint16_t readWord(const uint8_t device, const uint8_t command);         // Read a word from DS1631          //
      uint8_t  writeByte(const uint8_t device, const uint8_t command,         // Write a byte to a device/address //
                         const uint8_t data);                                 //                                  //
      uint8_t  writeWord(const uint8_t device, const uint8_t command,         // Write a word to a device/address //
                         const int16_t data);                                 //                                  //
  }; // of DS1631 class definition                                            //                                  //
#endif                                                                        //----------------------------------//