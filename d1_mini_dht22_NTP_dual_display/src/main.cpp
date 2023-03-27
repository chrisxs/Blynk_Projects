#define BLYNK_PRINT Serial

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <time.h>

#include "blynk_setting.h"
#include "OLED_setting.h"
#include "OTA_setting.h"
#include "webserial_setting.h"


int timezone = 8 * 3600; // 设置时区，这里设置为东八区
int dst = 0;

DHT dht(DHTPIN, DHTTYPE);

void sendSensor()
{
  float h = dht.readHumidity();    // 读取湿度值
  float t = dht.readTemperature(); // 读取温度值
  Blynk.virtualWrite(V0, t);       // 将温度值写入虚拟引脚 V0
  Blynk.virtualWrite(V1, h);       // 将湿度值写入虚拟引脚 V1
  
  Blynk.virtualWrite(V2, "IP地址: ", WiFi.localIP().toString());             // 在Blynk app上显示本地IP地址
  Blynk.virtualWrite(V3, "MAC地址: ", WiFi.macAddress());                    // 在Blynk app上显示MAC地址
  Blynk.virtualWrite(V4, "RSSI: ", WiFi.RSSI(), " ", "SSID: ", WiFi.SSID()); // 在Blynk app上显示WiFi信号强度和SSID

 ///web和物理串口打印信息///
//    Serial.println("温度：" + String(t));
//    Serial.println("湿度：" + String(h));
//    WebSerial.println("温度：" + String(t));
//    WebSerial.println("湿度：" + String(h));
}

BLYNK_WRITE(V5)
{
  if (param.asInt() == 0) // 如果传入的参数值为 0
  {
    display.displayOff();  // 关闭显示屏1
    display2.displayOff(); // 关闭显示屏2
    Serial.println("屏幕已经关闭");
    WebSerial.println("屏幕已经关闭");
  }
  else // 如果传入的参数值不为 0
  {
    display.displayOn();  // 打开显示屏1
    display2.displayOn(); // 打开显示屏2
    Serial.println("屏幕已经开启");
    WebSerial.println("屏幕已经开启");
  }
}



void setup()
{
  // 启动串口调试
  Serial.begin(115200);

  WebSerial.begin(&serialserver);
  WebSerial.msgCallback(recvMsg); // 附加反馈信息
  serialserver.begin();           // serialserver启动

  OTA();

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
    WebSerial.println("连接成功，IP地址: " + (String(WiFi.localIP().toString())));
    WebSerial.println("blynk token: " + String(auth) + "\nssid: " + String(ssid) + "\nblynk_server: " + String(blynk_server) + "\nblynk_port:" + String(blynk_port));
  }
  else
  {
    // 如果连接失败，打印错误信息
    Serial.print("WiFi连接失败");
  }
}

void loop()
{
  Blynk.run();  // 运行Blynk
  timer.run();  // 运行Blynk定时器
  draw_time();  // 显示时间
  draw_DHT22(); // 显示DHT22传感器数据
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

void recvMsg(uint8_t *data, size_t len) // 接收数据的函数
{
  WebSerial.println("接收到数据..."); // 打印一条提示信息，表示已接收到数据
  String d = "";                      // 创建一个空字符串变量 d，用于存储接收到的数据
  for (int i = 0; i < len; i++)       // 循环读取接收到的数据
  {
    d += char(data[i]); // 将接收到的字节数据转换为字符并拼接到字符串变量 d 中
  }
  WebSerial.println(d); // 打印接收到的数据
  /// 串口诊断命令,web串口收到“debug”就打印相关信息///
  if (d == "debug")
  {
    WebSerial.println("\n收到debug命令\n");
    WebSerial.println("blynk token: " + String(auth) + "\nssid: " + String(ssid) + "\nip: " + String(WiFi.localIP().toString()) + "\nblynk_server: " + String(blynk_server) + "\nblynk_port: " + String(blynk_port));
    Serial.println("\n收到debug命令\n");
    Serial.println("blynk token: " + String(auth) + "\nssid: " + String(ssid) + "\nip: " + String(WiFi.localIP().toString()) + "\nblynk_server: " + String(blynk_server) + "\nblynk_port: " + String(blynk_port));
  }
}