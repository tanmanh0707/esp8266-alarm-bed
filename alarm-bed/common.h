#pragma once

#include <SPI.h>
#include <FS.h>
#include <HX711.h>

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define BUZZER_PIN                    D8

/* TIME */
#define TIME_SECOND                   1000    //ms
#define TIME_MINUTE                   (60 * TIME_SECOND)
#define TIME_HOUR                     (60 * TIME_MINUTE)

/* WiFi */
#define WIFI_CREDENTIALS_LEN          32
#define WIFI_AP_SSID                  "Alarm Bed ESP8266"
#define WIFI_AP_PASSWORD              ""
#define WIFI_CONNECT_TIMEOUT          (10 * TIME_SECOND)

/* Web Server/Socket */
#define WEBSERVER_PORT                80
#define WEBSOCKET_PATH                "/ws"

/* TIMEZONE
 * Please get your timezone offset here
 * https://www.epochconverter.com/timezones
*/
#define TIMEZONE_OFFSET         25200

void WEBSOCKET_Send(String text);

/* Database */
void DB_Init();
void DB_Store();
const char *DB_GetWifiSSID();
const char *DB_GetWifiPass();
void DB_UpdateWifiCredentials(const char *ssid, const char *pass);
void DB_UpdateWifiCredentials(String ssid, String pass);
void DB_UpdateTimer(uint8_t on_h, uint8_t on_m);
void DB_GetTimerOn(uint8_t &hour, uint8_t &min);

/* File System */
bool STORAGE_ReadFile(fs::FS &fs, const char * path, uint8_t *data, size_t size);
bool STORAGE_WriteFile(fs::FS &fs, const char * path, const uint8_t *data, size_t size);
bool STORAGE_FileExist(fs::FS &fs, const char * path);
bool STORAGE_FileExist(fs::FS &fs, String path);

/* Time Client */
void TimeClient_Init();
void TimeClient_Task();
void TimeClient_Update();
bool TimeClient_IsUpdated();
void TimeClient_GetTime(uint8_t &hrs, uint8_t &min);

void WIFI_Init();
