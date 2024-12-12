#include "common.h"
#include <WiFiUdp.h>
#include <NTPClient.h>

/********************************************************************
 Definitions and macros
 ********************************************************************/
#define SERIAL_PRINT    Serial.print
#define SERIAL_PRINTF    Serial.printf
#define SERIAL_PRINTLN    Serial.println
#define LOCAL_PRINT     SERIAL_PRINT
#define LOCAL_PRINTF    SERIAL_PRINTF
#define LOCAL_PRINTLN   SERIAL_PRINTLN

/********************************************************************
 Local Variables
 ********************************************************************/
unsigned long UPDATE_TIME_DURATION = 60000;
static bool g_isTimeUpdated = false;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", TIMEZONE_OFFSET, (UPDATE_TIME_DURATION / 2));
/********************************************************************
 Local functions
 ********************************************************************/
void TimeClient_Init()
{
  timeClient.begin();
}

void TimeClient_Task()
{
  static unsigned long lupdate_time = 0;

  {
    if (g_isTimeUpdated == false || millis() - lupdate_time > UPDATE_TIME_DURATION)
    {
      TimeClient_Update();
      lupdate_time = millis();
    }
  }
}

bool TimeClient_IsUpdated()
{
  return g_isTimeUpdated;
}

void TimeClient_Update()
{
  if (timeClient.update())
  {
    // LOCAL_PRINTF(("TIME => %02d:%02d\n", timeClient.getHours(), timeClient.getMinutes()));
    Serial.print("TIME => "); Serial.print(timeClient.getHours()); Serial.print(":");Serial.println(timeClient.getMinutes());
    UPDATE_TIME_DURATION = (60 - timeClient.getSeconds()) * 1000;
    g_isTimeUpdated = true;
  }
  else
  {
    // LOCAL_PRINTF(("TIME => Failed\n"));
    Serial.println("TIME => Failed!");
    g_isTimeUpdated = false;
  }
}

void TimeClient_GetTime(uint8_t &hrs, uint8_t &min)
{
  hrs = timeClient.getHours();
  min = timeClient.getMinutes();
  int epochTime = timeClient.getEpochTime();
//   LOCAL_PRINTF(("TIME => %02d:%02d\n", currentHour, currentMinute));
    // Serial.print("TIME => "); Serial.print(currentHour); Serial.print(":");Serial.println(currentMinute);
}

