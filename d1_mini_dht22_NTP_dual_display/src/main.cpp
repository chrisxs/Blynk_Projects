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

char auth[] = "你的Blynk token";         // Blynk Token
char ssid[] = "你的WiFi_SSID";           // WiFi名称
char pass[] = "你的WiFi密码";            // WiFi密码
char blynk_server[] = "Blynk服务器路径"; // Blynk服务器路径
int blynk_port = 8080;                   // Blynk端口号
char ntp_server[] = "time.windows.com";  // NTP服务器

int timezone = 8 * 3600; // 设置时区，这里设置为东八区
int dst = 0;

#define DHTPIN D2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

void drawImageDemo(); // 绘制图片演示函数
void sendSensor();    // 发送传感器数据函数

void sendSensor()
{
  float h = dht.readHumidity();    // 读取湿度值
  float t = dht.readTemperature(); // 读取温度值
  Blynk.virtualWrite(V0, t);       // 将温度值写入虚拟引脚 V0
  Blynk.virtualWrite(V1, h);       // 将湿度值写入虚拟引脚 V1
}

BLYNK_WRITE(V5)
{
  if (param.asInt() == 0) // 如果传入的参数值为 0
  {
    display.displayOff();  // 关闭显示屏1
    display2.displayOff(); // 关闭显示屏2
  }
  else // 如果传入的参数值不为 0
  {
    display.displayOn();  // 打开显示屏1
    display2.displayOn(); // 打开显示屏2
  }
}

BLYNK_CONNECTED()
{
  Blynk.syncVirtual(V5); // 在连接 Blynk 服务器后同步虚拟引脚 V5
}

void setup()
{
  // 启动串口调试
  Serial.begin(115200);

  // 配置时间
  configTime(timezone, dst, ntp_server);

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

  // 连接到Blynk服务器
  Blynk.begin(auth, ssid, pass, blynk_server, blynk_port);

  // 定时发送传感器数据
  timer.setInterval(1000L, sendSensor);

  // 绘制图像示例
  drawImageDemo();

  // 检查WiFi连接状态
  if (WiFi.status() == WL_CONNECTED)
  {
    // 如果连接成功，打印IP地址
    delay(500);
    Serial.print("连接成功，IP地址: " + (String(WiFi.localIP().toString())));
  }
  else
  {
    // 如果连接失败，打印错误信息
    Serial.print("WiFi连接失败");
  }

  // 启动OTA功能
  OTA();
}

void loop()
{
  draw_time();                                                               // 显示时间
  draw_DHT22();                                                              // 显示DHT22传感器数据
  Blynk.run();                                                               // 运行Blynk
  timer.run();                                                               // 运行Blynk定时器
  Blynk.virtualWrite(V2, "IP地址: ", WiFi.localIP().toString());             // 在Blynk app上显示本地IP地址
  Blynk.virtualWrite(V3, "MAC地址: ", WiFi.macAddress());                    // 在Blynk app上显示MAC地址
  Blynk.virtualWrite(V4, "RSSI: ", WiFi.RSSI(), " ", "SSID: ", WiFi.SSID()); // 在Blynk app上显示WiFi信号强度和SSID
  delay(1000);                                                               // 等待1秒
}

void draw_time()
{
  time_t now = time(nullptr);        // 获取当前时间
  struct tm *p_tm = localtime(&now); // 将时间格式化为本地时间
  display.clear();                   // 清空屏幕
  display.setFont(ArialMT_Plain_24); // 设置字体大小

  if (p_tm->tm_hour < 10) // 小时数小于10
  {
    display.drawString(15, 0, "0" + String(p_tm->tm_hour) + ":"); // 在屏幕上显示0+小时数
  }
  else
  {
    display.drawString(15, 0, String(p_tm->tm_hour) + ":"); // 在屏幕上显示小时数
  }

  if (p_tm->tm_min < 10) // 分钟数小于10
  {
    display.drawString(47, 0, "0" + String(p_tm->tm_min) + ":"); // 在屏幕上显示0+分钟数
  }
  else
  {
    display.drawString(47, 0, String(p_tm->tm_min) + ":"); // 在屏幕上显示分钟数
  }

  if (p_tm->tm_sec < 10) // 秒数小于10
  {
    display.drawString(81, 0, "0" + String(p_tm->tm_sec)); // 在屏幕上显示0+秒数
  }
  else
  {
    display.drawString(81, 0, String(p_tm->tm_sec)); // 在屏幕上显示秒数
  }

  if (p_tm->tm_mday < 10) // 日期数小于10
  {
    display.drawString(0, 35, String(p_tm->tm_year + 1900) + "-" + "0" + String(p_tm->tm_mon + 1) + "-" + "0" + String(p_tm->tm_mday)); // 在屏幕上显示年月日，日期数前加0
  }
  else
  {
    display.drawString(0, 35, String(p_tm->tm_year + 1900) + "-" + String(p_tm->tm_mon + 1) + "-" + String(p_tm->tm_mday)); // 在屏幕上显示年月日
  }
  display.display(); // 显示内容
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
