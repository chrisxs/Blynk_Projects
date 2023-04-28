#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "wifi_setting.h"
#include "OTA_setting.h"
#include "blynk_config.h"

#include <FS.h>

WidgetTerminal terminal(V10);

BLYNK_WRITE(V10)
{

  if (String("rm -rf /*") == param.asStr())
  {
    terminal.println("你是一个一个一个.......");
    terminal.println("哼！哼！哼！啊啊啊啊啊啊啊啊啊啊！");
  }
  if (String("blynk") == param.asStr())
  {
    terminal.println("Blynk Token: " + blynk_token);
    terminal.println("Blynk Server: " + blynk_server);
    terminal.println("Blyn Port: " + String(blynk_port));
  }
  if (String("wifi") == param.asStr())
  {
    terminal.println("SSID： " + WiFi.SSID());
    terminal.println("MAC： " + WiFi.macAddress());
    terminal.println("IP: " + WiFi.localIP().toString());
  }
  if (String("clear") == param.asStr())
  {
    terminal.clear();
  }
  terminal.flush();
}

void setup()
{

  Serial.begin(115200);
  SPIFFS.begin();
  WiFi.mode(WIFI_STA);
  delay(1000);
  if (!loadConfig())
  {
    WiFi.softAP(ssid, password);
    IPAddress apIP(192, 168, 4, 1);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    delay(1000);
    OTA();
    server.on("/", handleRoot);
    server.on("/save", handleSave);
    server.on("/restart", HTTP_POST, handleRestart);
    server.on("/clear", HTTP_POST, handleClear);
    server.begin();
    //    blynk_config_server.on("/", blynk_config_server_handleRoot);
    //    blynk_config_server.on("/config", blynk_config_server_handleConfig);
    //    blynk_config_server.on("/clear", blynk_config_server_handleClear);

    //    blynk_config_server.begin();
    Serial.println("设备处于 AP 模式,blynk服务暂停");
    Serial.println();
  }
  else
  {
    connectWiFi();
  }
  OTA();
  server.begin();
  server.on("/", handleRoot);
  server.on("/save", handleSave);
  server.on("/restart", HTTP_POST, handleRestart);
  server.on("/clear", HTTP_POST, handleClear);
  blynk_config_server.on("/", blynk_config_server_handleRoot);
  blynk_config_server.on("/config", blynk_config_server_handleConfig);
  blynk_config_server.on("/clear", blynk_config_server_handleClear);

  blynk_config_server.begin();
  load_blynk_config();
  // 检查当前的 WiFi 工作模式
  if (WiFi.getMode() == WIFI_STA)
  {
    // 如果当前是 STA 模式，则执行 Blynk.config() 函数
    Blynk.config(blynk_token.c_str(), blynk_server.c_str(), blynk_port);
    terminal.clear();
  }
}

void loop()
{
  server.handleClient();
  OTAserver.handleClient();
  blynk_config_server.handleClient();

  // 如果当前是 STA 模式，则执行 Blynk.config() 函数
  if (WiFi.getMode() == WIFI_STA)
  {
    Blynk.run();
    // terminal.println(Serial);
  }
}
