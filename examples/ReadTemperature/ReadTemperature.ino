/*!

 @file ReadTemperature.ino

 @section ReadTemperature_intro_section Description

 Example program for using the DS1631 library which allows access to the DS1631 I2C thermometer. The
library as well as the most current version of this program is available at GitHub using the address
 https://github.com/Zanduino/DS1631 and a more detailed description of this program (and the
library) can be found at https://github.com/Zanduino/DS1631/wiki/ReadTemperature.ino \n\n

 The datasheet for the DS1631 can be found at
https://datasheets.maximintegrated.com/en/ds/DS1631-DS1731.pdf

 @section ReadTemperaturelicense GNU General Public License v3.0

 This program is free software: you can redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version. This program is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details. You should have
received a copy of the GNU General Public License along with this program.  If not, see
<http://www.gnu.org/licenses/>.

 @section ReadTemperatureauthor Author

Written by Arnd <Zanshin_Github@sv-zanshin.com> / https://www.github.com/SV-Zanshin

 @section ReadTemperatureversions Changelog

 Version | Date       | Developer  | Comments
 ------- | ---------- | ---------- | ---------------------------------------------------
 1.0.2   | 2020-12-12 | SV-Zanshin | Reformatted for clang-format
 1.0.1   | 2019-02-03 | SV-Zanshin | Issue #7 - convert documentation to Doxygen
 1.0.0   | 2018-06-28 | SV-Zanshin | Minor cosmetic changes to comments
 1.0.0   | 2017-08-23 | SV-Zanshin | Completed
 1.0.0b  | 2017-08-20 | SV-Zanshin | Initial coding
*/
#include <DS1631.h>  // Include the DS1631 library

/***************************************************************************************************
** Declare all program constants                                                                  **
***************************************************************************************************/
const uint32_t SERIAL_SPEED{115200};  ///< Set the baud rate for Serial I/O

/***************************************************************************************************
** Declare global variables and instantiate classes                                               **
***************************************************************************************************/

DS1631_Class DS1631;  ///< Create a class instance for the DS1631

void setup() {
  /*!
    @brief    Arduino method called once at startup to initialize the system
    @details  This is an Arduino IDE method which is called first upon boot or restart. It is only
              called one time and then control goes to the main "loop()" method, from which control
              never returns
    @return   void
  */
  Serial.begin(SERIAL_SPEED);
#ifdef __AVR_ATmega32U4__  // If a 32U4 processor, then wait for serial interface to initialize
  delay(3000);
#endif
  Serial.print(F("\nStarting ReadTemperature program\n"));
  Serial.print(F("- Compiled with c++ version "));
  Serial.print(F(__VERSION__));
  Serial.print(F("\n- On "));
  Serial.print(F(__DATE__));
  Serial.print(F(" at "));
  Serial.print(F(__TIME__));
  Serial.print(F("\n"));
  while (!DS1631.begin(I2C_STANDARD_MODE))  // Initialize RTC communications
  {
    Serial.println(F("Unable to find a DS1631. Retrying in 3 seconds."));
    delay(3000);
  }  // of loop until device is located
  Serial.print(F("Found "));
  Serial.print(DS1631.thermometers);
  Serial.println(F(" DS1631 device(s)"));
  Serial.println(
      F("Setting alarms when temperature goes up 1\xC2\xB0"
        "C"));
  for (uint8_t i = 0; i < DS1631.thermometers; i++)  // loop for each thermometer
  {
    DS1631.setPrecision(i, 12);  // Set maximum precision 12bit, sensitivity 0.0625�C
    DS1631.setContinuous(i);     // Activate continuous mode
    DS1631.setAlarmTemperature(i, 1,
                               DS1631.readTemp(i) + 16);  // Trigger alarm when temp goes up 1�C
  }                                                       // of for-next every thermometer found
}  // of method setup()

void loop() {
  /*!
    @brief    Arduino method for the main program loop
    @details  This is the main program for the Arduino IDE, it is an infinite loop and keeps on
              repeating.
    @return   void
  */
  for (uint8_t i = 0; i < DS1631.thermometers; i++) {
    Serial.print(F("Thermometer "));
    Serial.print(i + 1);
    if (DS1631.getAlarm(i))  // Display if alarm raised
    {
      Serial.print(F(" alarm raised. Thermometer "));
    }  // of if-then an alarm has been raised
    Serial.print(F(" = "));
    Serial.print(DS1631.readTemp(i) * 0.0625,
                 4);  // convert internal to Celsius units using floating point
    Serial.println(
        F("\xC2\xB0"
          "C"));
  }  // of for-next each thermometer
  Serial.println();
  Serial.print(F("Waiting 5 Seconds..."));
  delay(5000);
  Serial.println();
}  // of method loop()
