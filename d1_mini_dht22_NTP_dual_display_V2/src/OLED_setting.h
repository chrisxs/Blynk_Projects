#include <Wire.h>
#include "SSD1306Wire.h"

int timezone = 8 * 3600; // 设置时区，这里设置为东八区
int dst = 0;

void drawImageDemo(); // 绘制图片演示函数

void draw_time();  // 声明绘制时间的函数
void draw_DHT22(); // 声明绘制DHT22传感器数据的函数

SSD1306Wire display(0x3c, D4, D3);  // OLED屏幕1的I2C地址和引脚
SSD1306Wire display2(0x3c, D6, D5); // OLED屏幕2的I2C地址和引脚

#define WiFi_Logo_width 60
#define WiFi_Logo_height 36
// WiFi标志的二进制位数组，其中ProGMEM表示将数组存储在程序存储器中而不是在RAM中
const uint8_t WiFi_Logo_bits[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0x07, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xE0, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF,
  0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xFE, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
  0xFF, 0x03, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
  0x00, 0xFF, 0xFF, 0xFF, 0x07, 0xC0, 0x83, 0x01, 0x80, 0xFF, 0xFF, 0xFF,
  0x01, 0x00, 0x07, 0x00, 0xC0, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x0C, 0x00,
  0xC0, 0xFF, 0xFF, 0x7C, 0x00, 0x60, 0x0C, 0x00, 0xC0, 0x31, 0x46, 0x7C,
  0xFC, 0x77, 0x08, 0x00, 0xE0, 0x23, 0xC6, 0x3C, 0xFC, 0x67, 0x18, 0x00,
  0xE0, 0x23, 0xE4, 0x3F, 0x1C, 0x00, 0x18, 0x00, 0xE0, 0x23, 0x60, 0x3C,
  0x1C, 0x70, 0x18, 0x00, 0xE0, 0x03, 0x60, 0x3C, 0x1C, 0x70, 0x18, 0x00,
  0xE0, 0x07, 0x60, 0x3C, 0xFC, 0x73, 0x18, 0x00, 0xE0, 0x87, 0x70, 0x3C,
  0xFC, 0x73, 0x18, 0x00, 0xE0, 0x87, 0x70, 0x3C, 0x1C, 0x70, 0x18, 0x00,
  0xE0, 0x87, 0x70, 0x3C, 0x1C, 0x70, 0x18, 0x00, 0xE0, 0x8F, 0x71, 0x3C,
  0x1C, 0x70, 0x18, 0x00, 0xC0, 0xFF, 0xFF, 0x3F, 0x00, 0x00, 0x08, 0x00,
  0xC0, 0xFF, 0xFF, 0x1F, 0x00, 0x00, 0x0C, 0x00, 0x80, 0xFF, 0xFF, 0x1F,
  0x00, 0x00, 0x06, 0x00, 0x80, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x07, 0x00,
  0x00, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0xF8, 0xFF, 0xFF,
  0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0x01, 0x00, 0x00,
  0x00, 0x00, 0xFC, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF,
  0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0x1F, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x80, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};


void drawImageDemo()
{
  display2.clear();                                                           // 清除显示器
  display2.drawXbm(34, 5, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits); // 在坐标 (34, 5) 处绘制 WiFi_Logo_bits 图像
  display2.drawString(0, 40, "RSSI: " + String(WiFi.RSSI()) + " dB");         // 在坐标 (0, 40) 处显示 WiFi 信号强度
  display2.drawString(0, 50, "MAC: " + String(WiFi.macAddress()));            // 在坐标 (0, 50) 处显示 MAC 地址
  display2.display();                                                         // 刷新显示器
  display.clear();                                                            // 清除显示器
  display.setFont(ArialMT_Plain_10);                                          // 设置字体
  display.drawString(0, 0, "IP: " + String(WiFi.localIP().toString()));       // 在坐标 (0, 0) 处显示本地 IP 地址
  display.drawString(0, 20, "Hostname: " + String(WiFi.hostname()));          // 在坐标 (0, 20) 处显示主机名
  display.drawString(0, 40, "SSID: " + String(WiFi.SSID()));                  // 在坐标 (0, 40) 处显示 SSID
  display.display();                                                          // 刷新显示器
  delay(10000);                                                               // 延迟 10000 毫秒
}

void drawAPinfo()
{
  display2.clear();                                                           // 清除显示器
  display2.drawXbm(34, 5, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits); // 在坐标 (34, 5) 处绘制 WiFi_Logo_bits 图像
  display2.drawString(0, 50, "MAC: " + String(WiFi.macAddress()));            // 在坐标 (0, 50) 处显示 MAC 地址
  display2.display();                                                         // 刷新显示器
  display.clear();                                                            // 清除显示器
  display.setFont(ArialMT_Plain_10);                                          // 设置字体
  display.drawString(0, 0, "IP: 192.168.4.1");       // 在坐标 (0, 0) 处显示本地 IP 地址
  display.drawString(0, 20, "Hostname: " + String(WiFi.hostname()));          // 在坐标 (0, 20) 处显示主机名
  display.drawString(0, 40, "working in AP mode");                  // 在坐标 (0, 40) 处显示 SSID
  display.display();                                                          // 刷新显示器
  delay(10000);                                                               // 延迟 10000 毫秒
}

/*void draw_time()
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
*/

/*void draw_time()
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
    if (p_tm->tm_mon < 9) // 月份小于10
    {
      display.drawString(0, 35, String(p_tm->tm_year + 1900) + "-0" + String(p_tm->tm_mon + 1) + "-0" + String(p_tm->tm_mday)); // 在屏幕上显示年月日，日期数和月份数前加0
    }
    else
    {
      display.drawString(0, 35, String(p_tm->tm_year + 1900) + "-" + String(p_tm->tm_mon + 1) + "-0" + String(p_tm->tm_mday)); // 在屏幕上显示年月日，日期数前加0
    }
  }
  else
  {
    if (p_tm->tm_mon < 9) // 月份小于10
    {
      display.drawString(0, 35, String(p_tm->tm_year + 1900) + "-0" + String(p_tm->tm_mon + 1) + "-" + String(p_tm->tm_mday)); // 在屏幕上显示年月日，月份数前加0
    }
    else
    {
      display.drawString(0, 35, String(p_tm->tm_year + 1900) + "-" + String(p_tm->tm_mon + 1) + "-" + String(p_tm->tm_mday)); // 在屏幕上显示年月日
    }
  }
  display.display(); // 显示内容
}*/

void draw_time()
{
  time_t now = time(nullptr);        // 获取当前时间
  struct tm *p_tm = localtime(&now); // 将时间格式化为本地时间
  display.clear();                   // 清空屏幕
  display.setFont(ArialMT_Plain_24); // 设置字体大小

  // 格式化小时
  display.drawString(15, 0, (p_tm->tm_hour < 10 ? "0" : "") + String(p_tm->tm_hour) + ":");

  // 格式化分钟
  display.drawString(47, 0, (p_tm->tm_min < 10 ? "0" : "") + String(p_tm->tm_min) + ":");

  // 格式化秒钟
  display.drawString(81, 0, (p_tm->tm_sec < 10 ? "0" : "") + String(p_tm->tm_sec));

  // 格式化日期
  String date = String(p_tm->tm_year + 1900) + "-" + (p_tm->tm_mon < 9 ? "0" : "") + String(p_tm->tm_mon + 1) + "-" + (p_tm->tm_mday < 10 ? "0" : "") + String(p_tm->tm_mday);
  display.drawString(0, 35, date);

  display.display(); // 显示内容
}


