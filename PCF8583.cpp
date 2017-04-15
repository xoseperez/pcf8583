/*

  PCF8583 RTC and Event Counter Library for Arduino
  Copyright (C) 2013-2017 by Xose Pérez <xose dot perez at gmail dot com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "PCF8583.h"

// provide device address as a full 8 bit address (like the datasheet)
PCF8583::PCF8583(uint8_t address) {
    _address = address >> 1;  // convert to 7 bit so Wire doesn't choke
    Wire.begin();
}

void PCF8583::reset() {

    Wire.beginTransmission(_address);
    Wire.write(LOCATION_CONTROL);

    Wire.write((byte)0x04);    // 00 control/status (alarm enabled by default)
    Wire.write((byte)0x00);    // 01 set hundreds-of-seconds
    Wire.write((byte)0x00);    // 02 set second
    Wire.write((byte)0x00);    // 03 set minute
    Wire.write((byte)0x00);    // 04 set hour (24h format)
    Wire.write((byte)0x01);    // 05 set day
    Wire.write((byte)0x01);    // 06 set month
    Wire.write((byte)0x00);    // 07 set timer
    Wire.write((byte)0x00);    // 08 set alarm control
    Wire.write((byte)0x00);    // 09 set alarm hundreds-of-seconds
    Wire.write((byte)0x00);    // 0A set alarm second
    Wire.write((byte)0x00);    // 0B set alarm minute
    Wire.write((byte)0x00);    // 0C set alarm hour
    Wire.write((byte)0x01);    // 0D set alarm day
    Wire.write((byte)0x01);    // 0E set alarm month
    Wire.write((byte)0x00);    // 0F set alarm timer
    Wire.write((byte)0x00);    // 10 set year offset to 0
    Wire.write((byte)0x00);    // 11 set last read value for year to 0

    Wire.endTransmission();

}

void PCF8583::reset(uint8_t mode) {
    // TODO
}

void PCF8583::setMode(uint8_t mode) {
    uint8_t control = getRegister(LOCATION_CONTROL);
    control = (control & ~MODE_TEST) | (mode & MODE_TEST);
    setRegister(LOCATION_CONTROL, control);
}

uint8_t PCF8583::getMode() {
    return getRegister(LOCATION_CONTROL) & MODE_TEST;
}

int PCF8583::getMillisecond() {
    return (int) bcd2byte(getRegister(LOCATION_MILLISECONDS)) * 10;
}

uint8_t PCF8583::getSecond() {
    return bcd2byte(getRegister(LOCATION_SECONDS) & 0x7F);
}

uint8_t PCF8583::getMinute() {
    return bcd2byte(getRegister(LOCATION_MINUTES) & 0x7F);
}

uint8_t PCF8583::getHour() {
    return bcd2byte(getRegister(LOCATION_HOURS) & 0x3F);
}

unsigned long PCF8583::getTime() {
    unsigned long total = 0;
    total = getHour();
    total = total * 60 + getMinute();
    total = total * 60 + getSecond();
    total = total * 1000 + getMillisecond();
    return total;
}

char * PCF8583::getISOTime() {
    // TODO
}

void PCF8583::setTime(uint8_t sec, uint8_t min, uint8_t hour) {
    stop();
    Wire.beginTransmission(_address);
    Wire.write(LOCATION_SECONDS);
    Wire.write(byte2bcd(constrain(sec, 0, 59)));
    Wire.write(byte2bcd(constrain(min, 0, 59)));
    Wire.write(byte2bcd(constrain(hour, 0, 23)));
    Wire.endTransmission();
    start();
}

uint8_t PCF8583::getDay() {
    return bcd2byte(getRegister(LOCATION_DAY) & 0x3F);
}

uint8_t PCF8583::getMonth() {
    return bcd2byte(getRegister(LOCATION_MONTH) & 0x1F);
}

// To get the current year we have to look to 4 values!!
// 1) the top 2 bits of the 5th location
// 2) the last value requested for those digits
// 3) the offset value stored in the LOCATION_OFFSET_YEAR location
// 4) the base year (2000)
int PCF8583::getYear() {

    uint8_t year = getRegister(LOCATION_DAY) >> 6;
    uint8_t last = getRegister(LOCATION_LAST_YEAR);
    uint8_t offset = getRegister(LOCATION_OFFSET_YEAR);

    // we have changed the year: happy new year!!!
    if (last != year) {

        // we have overflowed the year value: update the offset
        if (last > year) {
            offset = (byte) (offset + 4);
            setRegister(LOCATION_OFFSET_YEAR, offset);
        }

        setRegister(LOCATION_LAST_YEAR, year);

    }

    return BASE_YEAR + offset + year;

}

uint8_t PCF8583::getWeekday() {
    return getRegister(LOCATION_MONTH) >> 5;
}

char * PCF8583::getISODate() {
    // TODO
}

void PCF8583::setDate(uint8_t day, uint8_t month, int year) {
    setDate(day, month, year, 0);
}

void PCF8583::setDate(uint8_t day, uint8_t month, int year, uint8_t weekday) {

    stop();

    // Calculate the 2 bit value for the year on top of the
    // BASE_YEAR and offset.
    // (BASE_YEAR + offset) has to be a leap year
    year -= BASE_YEAR;
    uint8_t offset = year & 0xFC;
    year -= offset;

    uint8_t value;
    Wire.beginTransmission(_address);
    Wire.write(LOCATION_DAY);
    value = byte2bcd(constrain(day, 1, 31)) | (year << 6);
    Wire.write(value);
    value = byte2bcd(constrain(month, 1, 12)) | ((constrain(weekday, 0, 6)) << 5);
    Wire.write(value);
    Wire.endTransmission();

    Wire.beginTransmission(_address);
    Wire.write(LOCATION_OFFSET_YEAR);
    Wire.write(offset);
    Wire.write(year);
    Wire.endTransmission();

    start();

}

void PCF8583::setDateTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t day, uint8_t month, int year) {
    setDateTime(sec, min, hour, day, month, year, 0);
}

void PCF8583::setDateTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t day, uint8_t month, int year, uint8_t weekday) {
    setTime(sec, min, hour);
    setDate(day, month, year, weekday);
}

// Only daily alarms support for now
void PCF8583::setAlarm(uint8_t sec, uint8_t min, uint8_t hour) {
    Wire.beginTransmission(_address);
    Wire.write(LOCATION_ALARM_SECONDS);
    Wire.write(byte2bcd(constrain(sec, 0, 59)));
    Wire.write(byte2bcd(constrain(min, 0, 59)));
    Wire.write(byte2bcd(constrain(hour, 0, 23)));
    Wire.endTransmission();
}

void PCF8583::setCountAlarm(unsigned long count) {
    // TODO
}

void PCF8583::enableAlarm() {
    uint8_t control;

    control = getRegister(LOCATION_CONTROL);
    control |= 0x04;
    setRegister(LOCATION_CONTROL, control);

    control = getRegister(LOCATION_ALARM_CONTROL);
    control |= 0x90;
    setRegister(LOCATION_ALARM_CONTROL, control);

}

void PCF8583::disableAlarm() {
    uint8_t control = getRegister(LOCATION_ALARM_CONTROL);
    control &= 0x6F;
    setRegister(LOCATION_ALARM_CONTROL, control);
}

void PCF8583::clearInterrupt() {
    uint8_t control = getRegister(LOCATION_ALARM_CONTROL);
    control &= 0x6F;
    setRegister(LOCATION_ALARM_CONTROL, control);
}

void PCF8583::setCount(unsigned long count) {
    stop();
    Wire.beginTransmission(_address);
    Wire.write(LOCATION_COUNTER);
    Wire.write(byte2bcd(count % 100));
    Wire.write(byte2bcd((count / 100) % 100));
    Wire.write(byte2bcd((count / 10000) % 100));
    Wire.endTransmission();
    start();
}

unsigned long PCF8583::getCount() {

    Wire.beginTransmission(_address);
    Wire.write(LOCATION_COUNTER);
    Wire.endTransmission();
    Wire.requestFrom(_address, (uint8_t) 3);

    unsigned long count = 0;
    count = bcd2byte(Wire.read());
    count = count + bcd2byte(Wire.read()) * 100L;
    count = count + bcd2byte(Wire.read()) * 10000L;

    return count;

}

// Private methods

void PCF8583::stop() {
    uint8_t control = getRegister(LOCATION_CONTROL);
    control |= 0x80;
    setRegister(LOCATION_CONTROL, control);
}

void PCF8583::start() {
    uint8_t control = getRegister(LOCATION_CONTROL);
    control &= 0x7F;
    setRegister(LOCATION_CONTROL, control);
}

void PCF8583::setRegister(uint8_t offset, uint8_t value) {
    Wire.beginTransmission(_address);
    Wire.write(offset);
    Wire.write(value);
    Wire.endTransmission();
}

uint8_t PCF8583::getRegister(uint8_t offset) {
    Wire.beginTransmission(_address);
    Wire.write(offset);
    Wire.endTransmission();
    Wire.requestFrom(_address, (uint8_t) 1);
    return Wire.read();
}

uint8_t PCF8583::bcd2byte(uint8_t value){
    return ((value >> 4) * 10) + (value & 0x0f);
}

uint8_t PCF8583::byte2bcd(uint8_t value){
    return ((value / 10) << 4) + (value % 10);
}
