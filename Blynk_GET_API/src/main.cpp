// 引入所需的库
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

#include <WiFiUdp.h>
#include <time.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson

#include "setting.h" // 自定义的一些常量和配置

#include <string>
#include <stdlib.h>

// 回调通知我们需要保存配置
void saveConfigCallback()
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void setup()
{
  Serial.begin(115200); // 设置串口通信的波特率为115200
  pinMode(SetPin, OUTPUT);
  int SetPinState = digitalRead(SetPin);
  display.init();                 // 初始化显示器
  display.setI2cAutoInit(true);   // 开启I2C自动初始化
  display.flipScreenVertically(); // 翻转屏幕以正确显示
  display.clear();                // 清除屏幕上的内容

  // 读取FS json的配置
  Serial.println("mounting FS...");
  if (SPIFFS.begin())
  {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json"))
    {
      // file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile)
      {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);

#ifdef ARDUINOJSON_VERSION_MAJOR >= 6
        DynamicJsonDocument json(1024);
        auto deserializeError = deserializeJson(json, buf.get());
        serializeJson(json, Serial);
        if (!deserializeError)
        {
#else
        DynamicJsonBuffer jsonBuffer;
        JsonObject &json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success())
        {
#endif
          Serial.println("\nparsed json");
          blynk_server = json["blynk_server"].as<const char *>();
          blynk_port = json["blynk_port"].as<const char *>();
          blynk_token = json["blynk_token"].as<const char *>();
        }
        else
        {
          Serial.println("failed to load json config");
        }
        configFile.close();
      }
    }
  }
  else
  {
    Serial.println("failed to mount FS");
  }
  // end read

  WiFiManagerParameter custom_blynk_server("server", "blynk server", blynk_server.c_str(), 40);
  WiFiManagerParameter custom_blynk_port("port", "blynk port", blynk_port.c_str(), 6);
  WiFiManagerParameter custom_blynk_token("blynk", "blynk token", blynk_token.c_str(), 32);
  WiFiManagerParameter custom_text("<p>点击SSID名称选择连接WiFi,并输入密码/服务器地址/设备口令</p>");

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  // set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  // add all your parameters here
  wifiManager.addParameter(&custom_blynk_server);
  wifiManager.addParameter(&custom_blynk_port);
  wifiManager.addParameter(&custom_blynk_token);
  wifiManager.addParameter(&custom_text);

  // 设置静态IP
  // wifiManager.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  // 在这里添加所有参数
  // wifiManager.addParameter(&custom_blynk_token);

  // 重置设置 - 用于测试
  // wifiManager.resetSettings();

  // 设置重置按钮的功能
  if (SetPinState == HIGH)
  {
    Serial.println("Getting Reset ESP Wifi-Setting.......");
    display.setFont(ArialMT_Plain_10);
    display.clear();
    display.drawXbm(35, 0, 60, 36, wifi_logo);
    display.drawString(0, 40, "RESET mode activated .");
    display.drawString(0, 50, "Please wait for reboot !");
    display.display();
    wifiManager.resetSettings();
    delay(5000);
    Serial.println("Formatting FS......");
    SPIFFS.format();
    delay(5000);
    Serial.println("Done Reboot In 5 seconds");
    display.setFont(ArialMT_Plain_16);
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

    ESP.restart();
  }

  // 设置最低信号质量,如果信号质量低于8%,则略这些AP
  // 默认值为:8%
  // wifiManager.setMinimumSignalQuality();

  // 设置超时直到配置关闭
  // 有用的是让它全部重试或进入睡眠状态
  // wifiManager.setTimeout(120);

  // 获取ssid并传递并尝试连接
  // 如果它没有连接，它将启动具有指定名称的访问点,"AutoConnectAP",并进入等待配置的阻塞循环
  if (!wifiManager.autoConnect("Get_Blynk_API", ""))
  {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    // 重置并重试，或者让它深入睡眠
    ESP.reset();
    delay(1000);
  }

  // 如果已经连接
  Serial.println("connected...yeey :)");

  blynk_server = custom_blynk_server.getValue();
  blynk_port = custom_blynk_port.getValue();
  blynk_token = custom_blynk_token.getValue();
  Serial.println("The values in the file are: ");
  Serial.println("\tblynk_server: " + String(custom_blynk_server.getValue()));
  Serial.println("\tblynk_port : " + String(custom_blynk_port.getValue()));
  Serial.println("\tblynk_token : " + String(custom_blynk_token.getValue()));

  if (shouldSaveConfig)
  {
    Serial.println("saving config");
#ifdef ARDUINOJSON_VERSION_MAJOR >= 6
    DynamicJsonDocument json(1024);
#else
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
#endif
    json["blynk_server"] = blynk_server.c_str();
    json["blynk_port"] = blynk_port.c_str();
    json["blynk_token"] = blynk_token.c_str();

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile)
    {
      Serial.println("failed to open config file for writing");
    }

#ifdef ARDUINOJSON_VERSION_MAJOR >= 6
    serializeJson(json, Serial);
    serializeJson(json, configFile);
#else
    json.printTo(Serial);
    json.printTo(configFile);
#endif
    configFile.close();
    // end save
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  ArduinoOTA.setHostname("Get_Blynk_API");
  ArduinoOTA.onStart([]()
                     {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else { // U_SPIFFS
        type = "filesystem";
      }
      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type); });
  ArduinoOTA.onEnd([]()
                   { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error)
                     {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      } });
  ArduinoOTA.begin();


  // WiFi.begin(ssid, password); // 连接WiFi网络
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
  ArduinoOTA.handle();
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
    http.begin(String(blynk_server.c_str()) + ":" + std::atoi(blynk_port.c_str()) + "/" + String(blynk_token.c_str()) + "/get/" + String(pin_v0));

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
      Serial.println("Temperature: " + String(number) + "\u00B0C \n");
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
      // Serial.println(String(url) + "/" + String(token) + "/get/" + String(pin_v0));
      Serial.println("HTTP request failed");
      Serial.println(String(blynk_server.c_str()) + ":" + std::atoi(blynk_port.c_str()) + "/" + String(blynk_token.c_str()) + "/get/" + String(pin_v0));
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
    http.begin(String(blynk_server.c_str()) + ":" + std::atoi(blynk_port.c_str()) + "/" + String(blynk_token.c_str()) + "/get/" + String(pin_v1));
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
      Serial.println("Humility: " + String(number) + "% \n");
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
    http.begin(String(blynk_server.c_str()) + ":" + std::atoi(blynk_port.c_str()) + "/" + String(blynk_token.c_str()) + "/get/" + String(pin_v2));
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
      Serial.println("Pressure: " + String(number) + "hPa \n");
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
    http.begin(String(blynk_server.c_str()) + ":" + std::atoi(blynk_port.c_str()) + "/" + String(blynk_token.c_str()) + "/get/" + String(pin_v3));
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
      Serial.println("Illumination: " + String(number) + "lUX \n");
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
