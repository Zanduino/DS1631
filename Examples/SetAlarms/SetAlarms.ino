/*******************************************************************************************************************
** Example program for using the DS1631 library which allows access to the DS1631 I2C thermometer. The libaray    **
** library as well as the most current version of this program is available at GitHub using the address           **
** https://github.com/SV-Zanshin/DS1631 and a more detailed description of this program (and the library) can be  **
** found at https://github.com/SV-Zanshin/DS1631/wiki/ReadTemperature.ino                                         **
**                                                                                                                **
** This program will take ambient temperature as the minimum possible temp, then have the thermometer heated by   **
** putting a finger on it to get a max temperature. Then the midpoint is selected and both the high and low       **
** alarms are set to that temperature. Then each alarm can be demonstrated by alternately heating the thermometer **
** and letting it cool back down.                                                                                 **
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
** 1.0.0b 2017-08-22 Arnd@SV-Zanshin.Com Initial coding                                                           **
**                                                                                                                **
*******************************************************************************************************************/
#include <DS1631.h>                                                          // Include the DS1631 library        //
/*******************************************************************************************************************
** Declare all program constants                                                                                  **
*******************************************************************************************************************/
const uint32_t SERIAL_SPEED        = 115200;                                  // Set the baud rate for Serial I/O //
const uint8_t  SPRINTF_BUFFER_SIZE =     32;                                  // Buffer size for sprintf()        //
const uint8_t  LED_PIN             =     13;                                  // Built-in Arduino green LED pin   //
/*******************************************************************************************************************
** Declare global variables and instantiate classes                                                               **
*******************************************************************************************************************/
DS1631_Class  DS1631;                                                         // Create an instance of the DS1631 //
char          inputBuffer[SPRINTF_BUFFER_SIZE];                               // Buffer for sprintf()/sscanf()    //
int32_t       ambientTemperature,maximumTemperature,alarmTemperature;         // Store min, max and alarm         //
/*******************************************************************************************************************
** Method Setup(). This is an Arduino IDE method which is called upon boot or restart. It is only called one time **
** and then control goes to the main loop, which loop indefinately.                                               **
*******************************************************************************************************************/
void setup() {                                                                // Arduino standard setup method    //
  pinMode(LED_PIN,OUTPUT);                                                    // Make the LED light an output pin //
  Serial.begin(SERIAL_SPEED);                                                 // Start serial port at Baud rate   //
  #ifdef  __AVR_ATmega32U4__                                                  // If this is a 32U4 processor, then//
    delay(3000);                                                              // wait 3 seconds for the serial    //
  #endif                                                                      // interface to initialize          //
  Serial.print(F("\nStarting SetAlarms program\n"));                          // Show program information         //
  Serial.print(F("- Compiled with c++ version "));                            //                                  //
  Serial.print(F(__VERSION__));                                               // Show compiler information        //
  Serial.print(F("\n- On "));                                                 //                                  //
  Serial.print(F(__DATE__));                                                  //                                  //
  Serial.print(F(" at "));                                                    //                                  //
  Serial.print(F(__TIME__));                                                  //                                  //
  Serial.print(F("\n"));                                                      //                                  //
  while (!DS1631.begin()) {                                                   // Initialize RTC communications    //
    Serial.println(F("Unable to find any DS1631. Checking again in 3s."));    // Show error text                  //
    delay(3000);                                                              // wait a second                    //
  } // of loop until device is located                                        //                                  //
  Serial.print(F("Found "));                                                  //                                  //
  Serial.print(DS1631.thermometers);                                          //                                  //
  Serial.println(F(" DS1631 device(s)"));                                     //                                  //
  for (uint8_t i=0;i<DS1631.thermometers;i++) {                               // For each thermometer             //
    DS1631.setPrecision(i,12);                                                // Set maximum precision = 0.0625°C //
  } // of for-next every thermometer found                                    //                                  //
  delay(750);                                                                 // Wait for measurements to be made //
  for (uint8_t i=0;i<DS1631.thermometers;i++) {                               // For each thermometer             //
    ambientTemperature += DS1631.readTemp(i);                                 // Read temperatures                //
  } // of for-next every thermometer found                                    //                                  //
  ambientTemperature = ambientTemperature / DS1631.thermometers;              // compute average                  //
  Serial.println(F("Now warm up the device(s) for 10 seconds"));              //                                  //
  delay(10000);                                                               // Wait for 10 seconds              //
  for (uint8_t i=0;i<DS1631.thermometers;i++) {                               // For each thermometer             //
    maximumTemperature += DS1631.readTemp(i);                                 // Read temperatures                //
  } // of for-next every thermometer found                                    //                                  //
  maximumTemperature = maximumTemperature / DS1631.thermometers;              // compute average                  //
  Serial.print(F("Ambient = "));                                              //                                  //
  Serial.print(ambientTemperature*0.0625,3);                                  //                                  //
  Serial.print(F("\xC2\xB0""C, Maximum = "));                                 //                                  //
  Serial.print(maximumTemperature*0.0625,3);                                  //                                  //
  Serial.println("\xC2\xB0""C");                                              //                                  //
  Serial.print(F("Setting High and Low alarms to "));                         //                                  //
  alarmTemperature = ambientTemperature+                                      // Set alarm to midpoint            //
                     ((maximumTemperature-ambientTemperature)/2);             //                                  //
  Serial.print(alarmTemperature*0.0625,3);                                    //                                  //
  Serial.println(F("\xC2\xB0""C"));                                           //                                  //
  for (uint8_t i=0;i<DS1631.thermometers;i++) {                               // For each thermometer             //
    DS1631.setAlarmTemperature(i,0,alarmTemperature);                         // Set low alarm                    //
    DS1631.setAlarmTemperature(i,1,alarmTemperature);                         // Set high alarm                   //
  } // of for-next every thermometer found                                    //                                  //
} // of method setup()                                                        //                                  //
/*******************************************************************************************************************
** This is the main program for the Arduino IDE, it is an infinite loop and keeps on repeating.                   **
*******************************************************************************************************************/
void loop() {                                                                 //                                  //
  static uint8_t alarmCode;                                                   //                                  //
  for (uint8_t i=0;i<DS1631.thermometers;i++) {                               // Loop through all thermometers    //
    Serial.print(F("#"));                                                     //                                  //
    Serial.print(i+1);                                                        //                                  //
    Serial.print(F(" = "));                                                   //                                  //
    Serial.print(DS1631.readTemp(i)*0.0625,4);                                // Display temperature              //
    Serial.print("\xC2\xB0""C ");                                             //                                  //
    alarmCode = DS1631.getAlarm(i);                                           // Get alarm state                  //
    if (alarmCode) {                                                          // see if an alarm has been raised  //
      if (alarmCode&1) Serial.print(F("(low)"));                              // Print out if low alarm raised    //
      if (alarmCode&2) Serial.print(F("(high)"));                             // Print out if low alarm raised    //
      Serial.print(F(" alarm raised."));                                      //                                  //
    }  // of if-then an alarm has been raised                                 //                                  //
    Serial.println();                                                         //                                  //
  } // of for-next each thermometer                                           //                                  //
  Serial.println();                                                           //                                  //
  delay(5000);                                                                // wait 5 seconds before looping    //
} // of method loop()                                                         //----------------------------------//