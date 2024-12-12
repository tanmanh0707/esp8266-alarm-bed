#include "common.h"
#include "html.h"

AsyncWebServer _server(WEBSERVER_PORT);
AsyncWebSocket _ws(WEBSOCKET_PATH);

bool LocalValidateWifiCredentials(String ssid, String pass)
{
  return (ssid.length() > 4 && (pass.length() >= 8 || pass.length() == 0));
}

bool LocalValidateTimer(int on_h, int on_m)
{
  return true;
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;

  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {

  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      // Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      // Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void WIFI_AccessPoint()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD);
  Serial.print("WiFi Access Point started! SSID: "); Serial.print(WIFI_AP_SSID);
  Serial.print(" - Password: "); Serial.println(WIFI_AP_PASSWORD);
}

void WIFI_Init()
{
  String ssid = DB_GetWifiSSID();
  String pass = DB_GetWifiPass();
  if (LocalValidateWifiCredentials(ssid, pass))
  {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    Serial.print("Connecting to: "); Serial.print(ssid); Serial.print(" - Pass: "); Serial.println(pass);

    unsigned long long lconnecttime = millis();
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      if (millis() - lconnecttime > WIFI_CONNECT_TIMEOUT) {
        Serial.println("");
        Serial.println("Cannot connect to WiFi. Switch to Access Point mode.");
        WIFI_AccessPoint();
        break;
      }
    }

    Serial.println("");
    Serial.print("Connected - Local IP address: "); Serial.println(WiFi.localIP().toString());
  }
  else
  {
    WIFI_AccessPoint();
  }

  /* Homepage */
  _server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", html_index_gz, sizeof(html_index_gz));
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  /* Configure */
  _server.on("/save-wifi", HTTP_POST, [](AsyncWebServerRequest *request) {
    String msg = "";
    if (request->hasParam("ssid") && request->hasParam("pw")) {
      String ssid = request->getParam("ssid")->value();
      String pw = request->getParam("pw")->value();
      if (LocalValidateWifiCredentials(ssid, pw)) {
        DB_UpdateWifiCredentials(ssid, pw);
        msg = "WiFi Credentials updated successfully!";
      } else { msg = "Invalid WiFi Credentials!"; }
    } else { msg = "Invalid request!"; }
    request->send(200, "text/plain", msg);
  });

  /* Set timer */
  _server.on("/set-timer", HTTP_POST, [](AsyncWebServerRequest *request) {
    String msg = "";
    if (request->hasParam("on-h") && request->hasParam("on-m")) {
      uint8_t on_h = request->getParam("on-h")->value().toInt();
      uint8_t on_m = request->getParam("on-m")->value().toInt();
      if (LocalValidateTimer(on_h, on_m)) {
        DB_UpdateTimer(on_h, on_m);
        msg = "Đặt thời gian thành công lúc " + String(on_h) + ":" + String(on_m);
        Serial.println(msg);
      } else { msg = "Invalid Timer!"; }
    } else { msg = "Invalid request!"; }
    request->send(200, "text/plain", msg);
  });

  /* Turn off motor */
  _server.on("/toggle-noti", HTTP_POST, [](AsyncWebServerRequest *request) {
    String msg = "Thay đổi hông báo thành công!";
    NOTIFICATION_Toggle();
    Serial.println(msg);
    request->send(200, "text/plain", msg);
  });

  /* WebSocket */
  _ws.onEvent(onEvent);
  _server.addHandler(&_ws);

  /* Start server */
  _server.begin();
}

void WEBSOCKET_Send(String text)
{
  _ws.textAll(text.c_str());
}
