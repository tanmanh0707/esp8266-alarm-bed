#include "common.h"

#define DB_PATH                       "/db_alarmbed"

typedef struct {
  char wifi_ssid[WIFI_CREDENTIALS_LEN];
  char wifi_pass[WIFI_CREDENTIALS_LEN];
  uint8_t on_hour;
  uint8_t on_min;
} Database_st;

static Database_st _db;

void DB_Init()
{
  SPIFFS.begin();

  memset((uint8_t*)&_db, 0, sizeof(_db));

  if (!STORAGE_FileExist(SPIFFS, DB_PATH)) {
    if (STORAGE_WriteFile(SPIFFS, DB_PATH, (const uint8_t*)&_db, sizeof(_db))){}
  }

  if (STORAGE_ReadFile(SPIFFS, DB_PATH, (uint8_t*)&_db, sizeof(_db)) == false) {
    // ESP_LOGE(LOG_COMMON, "Database read failed! Set to default.");
    memset((uint8_t*)&_db, 0, sizeof(_db));
    DB_Store();
  }

  Serial.print("Thời gian hẹn giờ: "); Serial.print(_db.on_hour); Serial.print(":");Serial.println(_db.on_min);
  Serial.println("Database init done!");
}

bool STORAGE_FileExist(fs::FS &fs, const char * path) {
  return fs.exists(path);
}

bool STORAGE_FileExist(fs::FS &fs, String path) {
  return STORAGE_FileExist(fs, path.c_str());
}

bool STORAGE_WriteFile(fs::FS &fs, const char * path, const uint8_t *data, size_t size)
{
  bool result = true;
  File file = fs.open(path, "w");

  if (!file){
    Serial.print("Failed to open file for writing: "); Serial.println(path);
    return false;
  }

  int writeSize = file.write(data, size);
  if (writeSize < size) {
    //ESP_LOGE(LOG_COMMON, "Write size error %d - expected %d", writeSize, size);
    Serial.print("Write size error: "); Serial.print(writeSize); Serial.print(" - expected ");Serial.println(size);
    result = false;
  }

  file.close();
  return result;
}

bool STORAGE_ReadFile(fs::FS &fs, const char * path, uint8_t *data, size_t size)
{
  bool result = true;
  File file = fs.open(path, "r");

  if (!file || file.isDirectory()) {
    Serial.print("Failed to open file for reading: "); Serial.println(path);
    return false;
  }

  if (file.available()) {
    int readSize = file.readBytes((char *)data, size);
    if (readSize < size) {
      Serial.print("Read size error: "); Serial.print(readSize); Serial.print(" - expected ");Serial.println(size);
      result = false;
    }
  }
  else {
    Serial.println("File not available. Read error!");
    result = false;
  }

  file.close();
  return result;
}

void DB_Store() {
  STORAGE_WriteFile(SPIFFS, DB_PATH, (uint8_t*)&_db, sizeof(_db));
}

void DB_UpdateWifiCredentials(const char *ssid, const char *pass) {
  if (ssid && pass) {
    memset(_db.wifi_ssid, 0, WIFI_CREDENTIALS_LEN);
    memset(_db.wifi_pass, 0, WIFI_CREDENTIALS_LEN);
    strncpy(_db.wifi_ssid, ssid, WIFI_CREDENTIALS_LEN - 1);
    strncpy(_db.wifi_pass, pass, WIFI_CREDENTIALS_LEN - 1);
    DB_Store();
  }
}

void DB_UpdateWifiCredentials(String ssid, String pass) {
  DB_UpdateWifiCredentials(ssid.c_str(), pass.c_str());
}

void DB_UpdateTimer(uint8_t on_h, uint8_t on_m)
{
  _db.on_hour = on_h;
  _db.on_min = on_m;
  DB_Store();
}

void DB_GetTimerOn(uint8_t &hour, uint8_t &min) {
  hour = _db.on_hour;
  min = _db.on_min;
}

const char *DB_GetWifiSSID() {
  return _db.wifi_ssid;
}

const char *DB_GetWifiPass() {
  return _db.wifi_pass;
}
