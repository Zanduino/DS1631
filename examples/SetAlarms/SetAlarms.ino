/*!

 @file SetAlarms.ino

 @section SetAlarms_intro_section Description

 Example program for using the DS1631 library which allows access to the DS1631 I2C thermometer. The
library as well as the most current version of this program is available at GitHub using the address
 https://github.com/Zanduino/DS1631 and a more detailed description of this program (and the
library) can be found at https://github.com/Zanduino/DS1631/wiki/ReadTemperature.ino \n\n

 This program will take ambient temperature as the minimum possible temp, then have the thermometer
heated by putting a finger on it to get a max temperature. Then the midpoint is selected and both
the high and low alarms are set to that temperature. Then each alarm can be demonstrated by
alternately heating the thermometer and letting it cool back down.

 The datasheet for the DS1631 can be found at
https://datasheets.maximintegrated.com/en/ds/DS1631-DS1731.pdf

 @section SetAlarmslicense GNU General Public License v3.0

 This program is free software: you can redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version. This program is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details. You should have
received a copy of the GNU General Public License along with this program.  If not, see
<http://www.gnu.org/licenses/>.

 @section SetAlarmsauthor Author

Written by Arnd <Zanshin_Github@sv-zanshin.com> / https://www.github.com/SV-Zanshin

 @section SetAlarmsversions Changelog

 Version | Date       | Developer  | Comments
 ------- | ---------- | ---------- | ---------------------------------------------------
 1.0.2   | 2020-12-12 | SV-Zanshin | Updated format to adhere to clang-format
 1.0.1   | 2019-02-02 | SV-Zanshin | Issue #7 - convert documentation to Doxygen
 1.0.0   | 2017-08-23 | SV-Zanshin | Completed testing and comments
 1.0.0b  | 2017-08-22 | SV-Zanshin | Initial coding
*/
#include <DS1631.h>  // Include the DS1631 library

/***************************************************************************************************
** Declare all program constants                                                                  **
***************************************************************************************************/
const uint32_t SERIAL_SPEED = 115200;  ///< Set the baud rate for Serial I/O

/***************************************************************************************************
** Declare global variables and instantiate classes                                               **
***************************************************************************************************/
DS1631_Class DS1631;              ///< Create an instance of the DS1631 class
int32_t      ambientTemperature,  ///< Store the ambient temperature
    maximumTemperature,           ///< Store the maximum temperature
    alarmTemperature;             ///< Store the minimum temperature

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
  Serial.print(F("\nStarting SetAlarms program\n"));
  Serial.print(F("- Compiled with c++ version "));
  Serial.print(F(__VERSION__));
  Serial.print(F("\n- On "));
  Serial.print(F(__DATE__));
  Serial.print(F(" at "));
  Serial.print(F(__TIME__));
  Serial.print(F("\n"));
  while (!DS1631.begin())  // Initialize RTC communications
  {
    Serial.println(F("Unable to find a DS1631. Checking again in 3 seconds."));
    delay(3000);
  }  // of loop until device is located
  Serial.print(F("Found "));
  Serial.print(DS1631.thermometers);
  Serial.println(F(" DS1631 device(s)"));
  for (uint8_t i = 0; i < DS1631.thermometers; i++)  // loop for each thermometer
  {
    DS1631.setPrecision(i, 12);  // Set maximum 12bit precision = 0.0625�C
    DS1631.setContinuous(i);     // Activate continuous mode
  }                              // of for-next every thermometer found
  delay(750);                    // Wait for measurements to be made
  for (uint8_t i = 0; i < DS1631.thermometers; i++) {
    ambientTemperature += DS1631.readTemp(i);  // Add readings
  }                                            // of for-next every thermometer found
  ambientTemperature = ambientTemperature / DS1631.thermometers;  // compute average
  Serial.println(F("Now warm up the device(s) for 10 seconds"));
  delay(10000);
  for (uint8_t i = 0; i < DS1631.thermometers; i++) {
    maximumTemperature += DS1631.readTemp(i);  // Add readings
  }                                            // of for-next every thermometer found
  maximumTemperature = maximumTemperature / DS1631.thermometers;  // compute average
  Serial.print(F("Ambient = "));
  Serial.print(ambientTemperature * 0.0625, 3);
  Serial.print(
      F("\xC2\xB0"
        "C, Maximum = "));
  Serial.print(maximumTemperature * 0.0625, 3);
  Serial.println(
      "\xC2\xB0"
      "C");
  Serial.print(F("Setting High and Low alarms to "));
  // Set alarm to midpoint of the readings
  alarmTemperature = ambientTemperature + ((maximumTemperature - ambientTemperature) / 2);
  Serial.print(alarmTemperature * 0.0625, 3);
  Serial.println(
      F("\xC2\xB0"
        "C\n\n"));
  for (uint8_t i = 0; i < DS1631.thermometers; i++) {
    DS1631.setAlarmTemperature(i, 0, alarmTemperature);  // Set low alarm
    DS1631.setAlarmTemperature(i, 1, alarmTemperature);  // Set high alarm
  }                                                      // of for-next every thermometer found
}  // of method setup()

void loop() {
  /*!
    @brief    Arduino method for the main program loop
    @details  This is the main program for the Arduino IDE, it is an infinite loop and keeps on
              repeating.
    @return   void
  */
  static uint8_t alarmCode;
  for (uint8_t i = 0; i < DS1631.thermometers; i++)  // Loop through all thermometers
  {
    Serial.print(F("DS1631 #"));
    Serial.print(i + 1);
    Serial.print(F(" = "));
    Serial.print(DS1631.readTemp(i) * 0.0625, 4);  // Display temperature in Celsius
    Serial.print(
        "\xC2\xB0"
        "C ");
    alarmCode = DS1631.getAlarm(i);  // Get alarm state
    if (alarmCode) {
      if (alarmCode & 1) Serial.print(F("(low)"));   // Print out if low alarm raised
      if (alarmCode & 2) Serial.print(F("(high)"));  // Print out if high alarm raised
      Serial.print(F(" alarm raised."));
    }  // of if-then an alarm has been raised
    Serial.println();
  }  // of for-next each thermometer
  Serial.println();
  delay(5000);
}  // of method loop()
