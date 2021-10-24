#include <Wire.h>
#include <RTClib.h>
#include <pcf8574.h>

RTC_PCF8563 RTC;
PCF8574 PCF_20(0x20);  // minutes

int c_hour;
int c_min;
int last_hour;
int i;

void setup()
{
//    Serial.begin(57600);
    Wire.begin();
    RTC.begin();
    for (i = 0; i < 14; i++)
      pinMode(i, OUTPUT);
    // following line sets the RTC to the date & time this sketch was compiled
    //RTC.adjust(DateTime(__DATE__, __TIME__));
    // loop through each light to test that they all work.
    last_hour = 14;
    for (i = 13; i > 0; i--)
    {
      digitalWrite(i, HIGH);
      delay(100);
    }
    for (i = 0; i < 8; i++)
    {
      PCF_20.write(i, 0);
      delay(100);
    }
    delay(500);
    for (i = 1; i < 14; i++)
    {
      digitalWrite(i, LOW);
      delay(100);
    }
    for (i = 7; i > -1; i--)
    {
      PCF_20.write(i, 1);
      delay(100);
    }
}

void loop()
{
    DateTime now = RTC.now();
    c_hour = now.hour();
    c_min = now.minute();
    if (last_hour == 14) last_hour = c_hour;
    // first, check AM or PM
    digitalWrite(13, (c_hour > 12 ? HIGH : LOW));
    // HOUR
    // change 24 hour into normal hour
    c_hour = (c_hour > 12 || c_hour == 0 ? abs(c_hour - 12) : c_hour);
    // turn off the previous hour.
    if (c_hour != last_hour)
    {
      digitalWrite(abs(last_hour - 12) + 1, LOW);
      last_hour = c_hour;
    }
    digitalWrite(abs(c_hour - 12) + 1, HIGH);
    // MINUTES
    // 0
    if (c_min == 0 || c_min < 5)
    {
      for (i = 0; i < 8; i++)
        PCF_20.write(i, 1);
    }
    // 5
    if (c_min > 4 && c_min < 10)
    {
      // don't need to turn off any LED since they were already turned off earlier
      PCF_20.write(0, 0);
    }
    // 10
    if (c_min > 10 && c_min < 15)
    {
      PCF_20.write(0, 1);
      PCF_20.write(1, 0);
    }
    // 15
    if (c_min > 14 && c_min < 20)
    {
      PCF_20.write(1, 0);
      PCF_20.write(2, 0);
    }
    // 20
    if (c_min > 20 && c_min < 25)
    {
      PCF_20.write(1, 1);
      PCF_20.write(2, 0);
    }
    // 25
    if (c_min > 24 && c_min < 30)
    {
      PCF_20.write(2, 1);
      PCF_20.write(3, 0);
    }
    // 30
    if (c_min > 29 && c_min < 35)
    {
      PCF_20.write(4, 0);
      PCF_20.write(3, 0);
    }
    // 35
    if (c_min > 34 && c_min < 40)
    {
      PCF_20.write(4, 0);
      PCF_20.write(3, 1);
    }
    // 40
    if (c_min > 39 && c_min < 45)
    {
      PCF_20.write(4, 1);
      PCF_20.write(5, 0);
    }
    // 45
    if (c_min > 44 && c_min < 50)
    {
      PCF_20.write(5, 0);
      PCF_20.write(6, 0);
    }
    // 50
    if (c_min > 49 && c_min < 55)
    {
      PCF_20.write(6, 0);
      PCF_20.write(5, 1);
    }
    // 55
    if (c_min > 54 && c_min != 0)
    {
      PCF_20.write(6, 1);
      PCF_20.write(7, 0);
    }
    delay(3000);
}
