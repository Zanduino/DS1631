/*******************************************************************************************************************
** Example program for using the DS1631 library which allows access to the DS1631 I2C thermometer. The libaray    **
** library as well as the most current version of this program is available at GitHub using the address           **
** https://github.com/SV-Zanshin/DS1631 and a more detailed description of this program (and the library) can be  **
** found at https://github.com/SV-Zanshin/DS1631/wiki/ReadTemperature.ino                                         **
**                                                                                                                **
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
** 1.0.0  2017-08-23 Arnd@SV-Zanshin.Com Completed                                                                **
** 1.0.0b 2017-08-20 Arnd@SV-Zanshin.Com Initial coding                                                           **
**                                                                                                                **
*******************************************************************************************************************/
#include <DS1631.h>                                                           // Include the DS1631 library       //
/*******************************************************************************************************************
** Declare all program constants                                                                                  **
*******************************************************************************************************************/
const uint32_t SERIAL_SPEED = 115200;                                         // Set the baud rate for Serial I/O //
/*******************************************************************************************************************
** Declare global variables and instantiate classes                                                               **
*******************************************************************************************************************/
DS1631_Class  DS1631;                                                         // Create an instance of the DS1631 //
/*******************************************************************************************************************
** Method Setup(). This is an Arduino IDE method which is called upon boot or restart. It is only called one time **
** and then control goes to the main loop, which loop indefinately.                                               **
*******************************************************************************************************************/
void setup() {                                                                // Arduino standard setup method    //
  Serial.begin(SERIAL_SPEED);                                                 // Start serial port at Baud rate   //
  #ifdef  __AVR_ATmega32U4__                                                  // If this is a 32U4 processor, then//
    delay(3000);                                                              // wait 3 seconds for the serial    //
  #endif                                                                      // interface to initialize          //
  Serial.print(F("\nStarting ReadTemperature program\n"));                    // Show program information         //
  Serial.print(F("- Compiled with c++ version "));                            //                                  //
  Serial.print(F(__VERSION__));                                               // Show compiler information        //
  Serial.print(F("\n- On "));                                                 //                                  //
  Serial.print(F(__DATE__));                                                  //                                  //
  Serial.print(F(" at "));                                                    //                                  //
  Serial.print(F(__TIME__));                                                  //                                  //
  Serial.print(F("\n"));                                                      //                                  //
  while (!DS1631.begin()) {                                                   // Initialize RTC communications    //
    Serial.println(F("Unable to find DS1631. Checking again in 3 seconds.")); // Show error text                  //
    delay(3000);                                                              // wait a second                    //
  } // of loop until device is located                                        //                                  //
  Serial.print(F("Found "));                                                  //                                  //
  Serial.print(DS1631.thermometers);                                          //                                  //
  Serial.println(F(" DS1631 device(s)"));                                     //                                  //
  Serial.println(F("Setting alarms when temperature goes up 1\xC2\xB0""C"));  //                                  //
  for (uint8_t i=0;i<DS1631.thermometers;i++) {                               // For each thermometer             //
    DS1631.setPrecision(i,12);                                                // Set maximum precision = 0.0625°C //
    DS1631.setContinuous(i);                                                  // Activate continuous mode         //
    DS1631.setAlarmTemperature(i,1,DS1631.readTemp(i)+16);                    // Alarm when temp goes up 1°C      //
  } // of for-next every thermometer found                                    //                                  //
} // of method setup()                                                        //                                  //
/*******************************************************************************************************************
** This is the main program for the Arduino IDE, it is an infinite loop and keeps on repeating.                   **
*******************************************************************************************************************/
void loop() {                                                                 //                                  //
  for (uint8_t i=0;i<DS1631.thermometers;i++) {                               //                                  //
    Serial.print(F("Thermometer "));                                          //                                  //
    Serial.print(i+1);                                                        //                                  //
    if (DS1631.getAlarm(i)) {                                                 // Print out if alarm raised        //
      Serial.print(F(" alarm raised. Thermometer "));                         //                                  //
    }  // of if-then an alarm has been raised                                 //                                  //
    Serial.print(F(" = "));                                                   //                                  //
    Serial.print(DS1631.readTemp(i)*0.0625,4);                                // convert internal to Celsius units//
    Serial.println("\xC2\xB0""C");                                            // using floating point             //
  } // of for-next each thermometer                                           //                                  //
  Serial.println();                                                           //                                  //
  delay(5000);                                                                //                                  //
} // of method loop()                                                         //----------------------------------//
