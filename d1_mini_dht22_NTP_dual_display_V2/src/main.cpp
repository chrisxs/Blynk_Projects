#define BLYNK_PRINT Serial
// #define BLYNK_DEBUG

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <time.h>

#include "wifi_setting.h"

#include "OLED_setting.h"

#include "OTA_setting.h"
#include "blynk_config.h"

#include <FS.h>

WidgetTerminal terminal(V10);
DHT dht(DHTPIN, DHTTYPE);

int timezone = 8 * 3600; // 设置时区，这里设置为东八区
int dst = 0;

void sendSensor()
{
  float h = dht.readHumidity();    // 读取湿度值
  float t = dht.readTemperature(); // 读取温度值
  Blynk.virtualWrite(V0, t);       // 将温度值写入虚拟引脚 V0
  Blynk.virtualWrite(V1, h);       // 将湿度值写入虚拟引脚 V1

  Blynk.virtualWrite(V2, "IP地址: ", WiFi.localIP().toString());             // 在Blynk app上显示本地IP地址
  Blynk.virtualWrite(V3, "MAC地址: ", WiFi.macAddress());                    // 在Blynk app上显示MAC地址
  Blynk.virtualWrite(V4, "RSSI: ", WiFi.RSSI(), " ", "SSID: ", WiFi.SSID()); // 在Blynk app上显示WiFi信号强度和SSID
}

BLYNK_WRITE(V5)
{
  if (param.asInt() == 0) // 如果传入的参数值为 0
  {
    display.displayOff();  // 关闭显示屏1
    display2.displayOff(); // 关闭显示屏2
    Serial.println("屏幕已经关闭");
  }
  else // 如果传入的参数值不为 0
  {
    display.displayOn();  // 打开显示屏1
    display2.displayOn(); // 打开显示屏2
    Serial.println("屏幕已经开启");
  }
}

// 用于诊断

BLYNK_WRITE(V10)
{
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
    // terminal.println("RSSI: " + WiFi.RSSI().toString());
    terminal.println("RSSI: " + String(WiFi.RSSI()));
  }
  if (String("reboot") == param.asStr())
  {
    ESP.restart();
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

  // 初始化显示屏1和2
  display.init();
  display2.init();

  // 设置显示屏1和2的I2C自动初始化
  display.setI2cAutoInit(true);
  display2.setI2cAutoInit(true);

  // 翻转显示屏1和2的竖直方向
  display.flipScreenVertically();
  display2.flipScreenVertically();

  // 设置显示屏1和2的字体和文本对齐方式
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display2.setFont(ArialMT_Plain_10);
  display2.setTextAlignment(TEXT_ALIGN_LEFT);

  // 启动DHT22传感器
  dht.begin();

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
    Serial.println("设备处于 AP 模式,blynk服务暂停");
    drawAPinfo();
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
    // 链接WiFi成功后，显示WiFi信息
    drawImageDemo();
    // 如果当前是 STA 模式，则执行 Blynk.config() 函数
    Blynk.config(blynk_token.c_str(), blynk_server.c_str(), blynk_port);
    // 定时发送传感器数据
    timer.setInterval(1000L, sendSensor);
    // 配置时间
    configTime(timezone, dst, ntp_server);
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
    Blynk.run();  // 运行Blynk
    timer.run();  // 运行Blynk定时器
    draw_time();  // 显示时间
    draw_DHT22(); // 显示DHT22传感器数据
  }
}

void draw_DHT22()
{
  float h = dht.readHumidity();                                 // 读取湿度值
  float t = dht.readTemperature();                              // 读取温度值
  display2.clear();                                             // 清空显示屏2
  display2.setFont(ArialMT_Plain_24);                           // 设置字体大小
  display2.drawString(15, 0, "T: " + String(t, 1) + "\u00B0C"); // 显示温度值
  display2.drawString(99, 0, "C");                              // 显示温度单位
  display2.drawString(15, 35, "H: " + String(h, 1) + " % ");    // 显示湿度值
  display2.display();                                           // 显示在屏幕上
}

