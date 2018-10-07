/*

  PCF8583 RTC and Event Counter Library for Arduino
  Copyright (C) 2013-2018 by Xose Pérez <xose dot perez at gmail dot com>

  The PCF8583 library is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  The PCF8583 library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with the PCF8583 library.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef PCF8583_h
#define PCF8583_h

#include <PCF8583.h>
#include "Arduino.h"
#include "Wire.h"

// Register offsets
#define LOCATION_CONTROL 0x00
#define LOCATION_MILLISECONDS 0x01
#define LOCATION_SECONDS 0x02
#define LOCATION_MINUTES 0x03
#define LOCATION_HOURS 0x04
#define LOCATION_DAY 0x05
#define LOCATION_MONTH 0x06
#define LOCATION_YEAR 0x07

#define LOCATION_ALARM_CONTROL 0x08
#define LOCATION_ALARM_SECONDS 0x10

#define LOCATION_COUNTER 0x01

#define LOCATION_OFFSET_YEAR 0x10
#define LOCATION_LAST_YEAR 0x11

// Function modes
#define MODE_CLOCK_32KHZ 0x00
#define MODE_CLOCK_50HZ 0x10
#define MODE_EVENT_COUNTER 0x20
#define MODE_TEST 0x30

// Weekdays
#define MONDAY 0
#define TUESDAY 1
#define WEDNESDAY 2
#define THURSDAY 3
#define FRIDAY 4
#define SATURDAY 5
#define SUNDAY 6

#define BASE_YEAR 2012
#define ALARM_ANY 99

class PCF8583 {

    public:

        PCF8583(uint8_t address);
        void reset(); // resets chip to factory values
        void reset(uint8_t mode); // resets specific registers

        void setMode(uint8_t mode);
        uint8_t getMode();

        // Time
        int getMillisecond();
        uint8_t getSecond();
        uint8_t getMinute();
        uint8_t getHour();
        unsigned long getTime(); // Returns milliseconds since midnight
        //char * getISOTime(); // Returns day time in HH:MM:SS
        void setTime(uint8_t sec, uint8_t min, uint8_t hour);

        // Date
        uint8_t getDay();
        uint8_t getMonth();
        int getYear();
        uint8_t getWeekday();
        //char * getISODate(); // Returns date in YYYY-MM-DD
        void setDate(uint8_t day, uint8_t month, int year);
        void setDate(uint8_t day, uint8_t month, int year, uint8_t weekday);

        // Date and time
        void setDateTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t day, uint8_t month, int year);
        void setDateTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t day, uint8_t month, int year, uint8_t weekday);

        // Alarm
        void setAlarm(uint8_t sec, uint8_t min, uint8_t hour);
        void setCountAlarm(unsigned long count);
        void enableAlarm(); // set alarm flag
        void disableAlarm(); // clear alarm flag
        void clearInterrupt(); // clear alarm interrupt

        // Event counter
        void setCount(unsigned long count);
        unsigned long getCount();

        // Low level access
        void setRegister(uint8_t offset, uint8_t value);
        uint8_t getRegister(uint8_t offset);

    private:

        void stop();
        void start();
        uint8_t byte2bcd(uint8_t value);
        uint8_t bcd2byte(uint8_t value);

        uint8_t _address;

};

#endif
