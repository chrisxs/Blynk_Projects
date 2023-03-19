// 引入所需的库
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "setting.h" // 自定义的一些常量和配置

// WiFi网络名称和密码
const char *ssid = "WiFi_SSID";
const char *password = "你的WiFi密码";

void get_v0(); // 获取第一个请求
void get_v1(); // 获取第二个请求
void get_v2(); // 获取第三个请求
void get_v3(); // 获取第四个请求

void setup()
{
  Serial.begin(115200);           // 设置串口通信的波特率为115200
  display.init();                 // 初始化显示器
  display.setI2cAutoInit(true);   // 开启I2C自动初始化
  display.flipScreenVertically(); // 翻转屏幕以正确显示
  display.clear();                // 清除屏幕上的内容

  WiFi.begin(ssid, password); // 连接WiFi网络
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..."); // 打印连接WiFi网络的提示信息
  }
  Serial.println("Connected to WiFi"); // 打印连接成功的提示信息

  display.drawXbm(35, 0, 60, 36, wifi_logo); // 在屏幕上绘制WiFi图标
  display.display();                         // 显示WiFi图标
  delay(5000);                               // 延迟5秒
}

// 这个函数是主循环，用于调用四个函数来获取四个不同的值，并延时 3 秒，然后再获取下一个值。
void loop()
{
  get_v0();
  delay(3000);
  get_v1();
  delay(3000);
  get_v2();
  delay(3000);
  get_v3();
  delay(3000);
}

/*函数用于发送 HTTP GET 请求，获取温度传感器的值。
函数中首先检查是否已经连接到 WiFi 网络，然后创建一个 HTTPClient 对象，开始发送 GET 请求，并使用从设置文件中读取的 PIN 号码和密钥参数。
如果请求成功，函数从返回的字符串中提取数字，并将其转换为 float 类型，然后在串口中打印温度值和 HTTP 状态码，最后在 OLED 屏幕上显示温度值，并延时 1 秒。
如果请求失败，函数会在串口中打印“HTTP request failed”，并在 OLED 屏幕上不显示任何内容。最后，函数会关闭 HTTPClient 对象，断开连接*/
void get_v0()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    // 发送GET请求
    http.begin(String(url) + "/" + String(token) + "/get/" + String(pin_v0));
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      Serial.println(payload);

      // 从返回的字符串中提取数字
      int start = payload.indexOf("\"") + 1;
      int end = payload.indexOf("\"", start);
      String numberString = payload.substring(start, end);
      float number = numberString.toFloat();
      Serial.println(httpCode);
      Serial.println("Temperature: " + String(number) + "\u00B0C");
      display.clear();
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setFont(ArialMT_Plain_24);
      display.drawString(30, 0, ("Temp"));
      display.drawString(20, 25, (String(number) + "\u00B0C"));
      display.display();
      delay(1000);
    }
    else
    {
      Serial.println("HTTP request failed");
    }

    // 断开连接
    http.end();
  }
}

void get_v1()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    // 发送GET请求
    http.begin(String(url) + "/" + String(token) + "/get/" + String(pin_v1));
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      Serial.println(payload);

      // 从返回的字符串中提取数字
      int start = payload.indexOf("\"") + 1;
      int end = payload.indexOf("\"", start);
      String numberString = payload.substring(start, end);
      float number = numberString.toFloat();
      Serial.println(httpCode);
      Serial.println("Humility: " + String(number) + "%");
      display.clear();
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setFont(ArialMT_Plain_24);
      display.drawString(20, 0, ("Humility"));
      display.drawString(20, 25, (String(number) + "%"));
      display.display();
      delay(1000);
    }
    else
    {
      Serial.println("HTTP request failed");
    }

    // 断开连接
    http.end();
  }
}

void get_v2()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    // 发送GET请求
    http.begin(String(url) + "/" + String(token) + "/get/" + String(pin_v2));
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      Serial.println(payload);

      // 从返回的字符串中提取数字
      int start = payload.indexOf("\"") + 1;
      int end = payload.indexOf("\"", start);
      String numberString = payload.substring(start, end);
      float number = numberString.toFloat();
      Serial.println(httpCode);
      Serial.println("Pressure: " + String(number) + "hPa");
      display.clear();
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setFont(ArialMT_Plain_24);
      display.drawString(15, 0, ("Pressure"));
      display.drawString(0, 25, (String(number) + "hPa"));
      display.display();
      delay(1000);
    }
    else
    {
      Serial.println("HTTP request failed");
    }

    // 断开连接
    http.end();
  }
}

void get_v3()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    // 发送GET请求
    http.begin(String(url) + "/" + String(token) + "/get/" + String(pin_v3));
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      Serial.println(payload);

      // 从返回的字符串中提取数字
      int start = payload.indexOf("\"") + 1;
      int end = payload.indexOf("\"", start);
      String numberString = payload.substring(start, end);
      float number = numberString.toFloat();
      Serial.println(httpCode);
      Serial.println("Illumination: " + String(number) + "lUX");
      display.clear();
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setFont(ArialMT_Plain_24);
      display.drawString(3, 0, ("Illumination"));
      display.drawString(5, 25, (String(number) + " Lux"));
      display.display();
      delay(1000);
    }
    else
    {
      Serial.println("HTTP request failed");
    }

    // 断开连接
    http.end();
  }
}
