//#include <Arduino.h>
#include <Wire.h> // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"

SSD1306Wire display(0x3c, D2, D1); // 设置OLED屏幕的名称/引脚/地址

void ShowAP_SSID()
{
  display.clear();
  display.drawString(0, 10, "AP-SSID:RobotArm");
  display.drawString(0, 20, "Password:none");
  display.display();
}

void RebootCountdown()
{
  display.clear();
  display.drawString(5, 25, "Reboot in 5 Sec !");
  display.display();
  delay(1000);

  display.clear();
  display.drawString(5, 25, "Reboot in 4 Sec !");
  display.display();
  delay(1000);

  display.clear();
  display.drawString(5, 25, "Reboot in 3 Sec !");
  display.display();
  delay(1000);

  display.clear();
  display.drawString(5, 25, "Reboot in 2 Sec !");
  display.display();
  delay(1000);

  display.clear();
  display.drawString(5, 25, "Reboot in 1 Sec !");
  display.display();
  delay(1000);
}

void ResetMode()
{
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.drawString(0, 40, "RESET mode activated .");
  display.drawString(0, 50, "Please wait for reboot !");
  display.display();
}

void drawinfo()
{
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.drawString(0, 0, "Hostname: " + String(WiFi.hostname()));
  display.drawString(0, 10, "RSSI: " + String(WiFi.RSSI()) + " dB");
  display.drawString(0, 20, "MAC: " + String(WiFi.macAddress()));
  display.drawString(0, 30, "IP: " + String(WiFi.localIP().toString()));
  display.drawString(0, 40, "SSID: " + String(WiFi.SSID()));
  display.display();
}