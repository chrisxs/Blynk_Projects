#define BLYNK_PRINT Serial

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <time.h>

#include "images.h"
#include "OTA_setting.h"

char auth[] = "6eb98047bbef4705ae8bc405d817e2a8";
char ssid[] = "chrisxs_01";
char pass[] = "aj20160223";
char blynk_server[] = "chrisxs.com";
char ntp_server[] = "time.windows.com";
int blynk_port = 8080;


int timezone = 8 * 3600; // 设置时区
int dst = 0;

#define DHTPIN D2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

void drawImageDemo();
void sendSensor();

void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // dht.readTemperature(true)
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
}

BLYNK_WRITE(V5)
{
  if (param.asInt() == 0)
  {
    display.displayOff();
    display2.displayOff();
  }
  else
  {
    display.displayOn();
    display2.displayOn();
  }
}

BLYNK_CONNECTED()
{
  Blynk.syncVirtual(V5);
}

void setup()
{
  // Debug console
  Serial.begin(115200);

  configTime(timezone, dst, ntp_server);

  display.init();
  display2.init();

  display.setI2cAutoInit(true);
  display2.setI2cAutoInit(true);

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  display2.flipScreenVertically();
  display2.setFont(ArialMT_Plain_10);
  display2.setTextAlignment(TEXT_ALIGN_LEFT);

  dht.begin();
  //Blynk.begin(auth, ssid, pass, IPAddress(121, 4, 166, 72), 8080);
    Blynk.begin(auth, ssid, pass, blynk_server, blynk_port);

  timer.setInterval(1000L, sendSensor);
  drawImageDemo();
  

  if (WiFi.status() == WL_CONNECTED)
  {
    delay(500);
    Serial.print("连接成功，IP地址: " + (String(WiFi.localIP().toString())));
  }else{
    Serial.print("WiFi连接失败");
  }
  OTA();
}

void loop()
{
  draw_time();
  draw_DHT22();
  Blynk.run();
  timer.run();
  Blynk.virtualWrite(V2, "IP地址: ", WiFi.localIP().toString());
  Blynk.virtualWrite(V3, "MAC地址: ", WiFi.macAddress());
  Blynk.virtualWrite(V4, "RSSI: ", WiFi.RSSI(), " ", "SSID: ", WiFi.SSID());
  delay(1000);
}



void draw_time()
{
  time_t now = time(nullptr);
  struct tm *p_tm = localtime(&now);
  display.clear();
  display.setFont(ArialMT_Plain_24);

  if (p_tm->tm_hour < 10)
  {
    display.drawString(15, 0, "0" + String(p_tm->tm_hour) + ":");
  }
  else
  {
    display.drawString(15, 0, String(p_tm->tm_hour) + ":");
  }

  if (p_tm->tm_min < 10)
  {
    display.drawString(47, 0, "0" + String(p_tm->tm_min) + ":");
  }
  else
  {
    display.drawString(47, 0, String(p_tm->tm_min) + ":");
  }

  if (p_tm->tm_sec < 10)
  {
    display.drawString(81, 0, "0" + String(p_tm->tm_sec));
  }
  else
  {
    display.drawString(81, 0, String(p_tm->tm_sec));
  }
  // if ((p_tm->tm_mday < 10) && ((p_tm->tm_mon + 1)<10))
  if (p_tm->tm_mday < 10)
  {
    display.drawString(0, 35, String(p_tm->tm_year + 1900) + "-" + "0" + String(p_tm->tm_mon + 1) + "-" + "0" + String(p_tm->tm_mday));
  }
  else
  {
    display.drawString(0, 35, String(p_tm->tm_year + 1900) + "-" + String(p_tm->tm_mon + 1) + "-" + String(p_tm->tm_mday));
  }
  display.display();
}

void draw_DHT22()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // dht.readTemperature(true)
  display2.clear();
  display2.setFont(ArialMT_Plain_24);
  display2.drawString(15, 0, "T: " + String(t, 1) + "\u00B0C");
  display2.drawString(99, 0, "C");
  display2.drawString(15, 35, "H: " + String(h, 1) + " % ");
  display2.display();
}
