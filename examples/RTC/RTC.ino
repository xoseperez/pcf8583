#include "PCF8583.h"

PCF8583 rtc(0xA0);

void assertEquals(const char * test, byte expected, byte real) {
    if (expected == real) {
        Serial.println("OK");
    } else {
        Serial.print("KO: ");
        Serial.println(test);
    }
}

void setup() {

    Serial.begin(9600);

    rtc.setTime(0, 10, 20);
    assertEquals("getSeconds", 0, rtc.getSecond());
    assertEquals("getMinutes", 10, rtc.getMinute());
    assertEquals("getHours", 20, rtc.getHour());

    delay(2000);
    assertEquals("getSeconds +2", 2, rtc.getSecond());

}

void loop() {}
