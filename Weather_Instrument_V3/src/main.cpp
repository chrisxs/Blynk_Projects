// #define BLYNK_PRINT Serial
//  #define BLYNK_DEBUG
#include <FS.h> //如果不正常就放到第一位
#include <Arduino.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson

#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>
#include <DNSServer.h>

#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP085.h>
#include <WEMOS_SHT3X.h>
#include <BH1750FVI.h>

#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ElegantOTA.h>

#include <string>
#include <stdlib.h>

WidgetTerminal terminal(V7);

Adafruit_BMP085 bmp;
SHT3X sht30(0x44);
BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);

const int ResetButton = D5;
const int LedPin = D4;
int ResetButtonState = digitalRead(ResetButton);
// int ResetButtonState = HIGH;

BlynkTimer timer;

// 用于WiFiManager界面中的变量服务器域名、端口、口令
std::string blynk_server;
std::string blynk_port;
std::string blynk_token;

ESP8266WebServer OTAserver(8266);

bool shouldSaveConfig = false;

// 回调通知我们需要保存配置
void saveConfigCallback()
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void sendSensor()
{
  if (sht30.get() == 0)
  {
    float h = sht30.humidity;
    float t = sht30.cTemp;
    Blynk.virtualWrite(V0, h);
    Blynk.virtualWrite(V1, t);
  }
  else
  {
    Serial.println("SHT3X Error!");
    terminal.println("SHT3X Error!");
  }

  if (!bmp.begin())
  {
    Serial.println("BMP085 Error!");
    terminal.println("BMP085 Error!");
  }
  else
  {
    float p = bmp.readPressure();
    Blynk.virtualWrite(V2, p / 100);
  }
  uint16_t lux = LightSensor.GetLightIntensity();
  float l = lux;
  Blynk.virtualWrite(V3, l);
}

BLYNK_WRITE(V7)
{

  // if you type "Marco" into Terminal Widget - it will respond: "Polo:"
  if (String("clear") == param.asStr())
  {
    terminal.clear();
  }

  // 确保所有内容都已发送
  terminal.flush();
}

void setup()
{
  digitalWrite(LedPin, HIGH);
  pinMode(ResetButton, INPUT_PULLUP);

  pinMode(LedPin, OUTPUT);
  Serial.begin(115200);
  bmp.begin();
  LightSensor.begin();

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
  if (ResetButtonState == LOW)
  {
    Serial.println("Getting Reset ESP Wifi-Setting.......");
    digitalWrite(LedPin, LOW);
    wifiManager.resetSettings();
    delay(5000);
    Serial.println("Formatting FS......");
    SPIFFS.format();
    delay(5000);
    Serial.println("Done Reboot In 5 seconds");
    delay(5000);
    digitalWrite(LedPin, HIGH);
    ESP.restart();
  }

  if (!wifiManager.autoConnect("4合1气象采集仪V3", ""))
  {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    // 重置并重试，或者让它深入睡眠
    ESP.reset();
    delay(1000);
  }

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

  ////////////OTA部分//////////////

  // 默认端口为"8266"
  // ArduinoOTA.setPort(8266);

  // 默认主机名为:esp8266-[ChipID]
  ArduinoOTA.setHostname("4合1气象采集仪V3-demo");

  // 默认情况下不验证
  // ArduinoOTA.setPassword("admin");
  // 密码也可以用它的md5值设置
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
  ArduinoOTA.onStart([]()
                     {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
    //注意：如果更新SPIFFS，这将是使用SPIFFS.end（）卸载SPIFFS的地方
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

  // Serial.println("Ready");
  // Serial.print("IP address: ");
  // Serial.println( WiFi.localIP().toString());
  // Serial.print("blynk server:  ");
  // Serial.println(blynk_server.c_str());
  // Serial.print("blynk port: ");
  // Serial.println(blynk_port.c_str());
  // Serial.print("blynk token: ");
  // Serial.println(blynk_token.c_str());

  Blynk.config(blynk_token.c_str(), blynk_server.c_str(), std::atoi(blynk_port.c_str()));
  timer.setInterval(1000L, sendSensor);
  ArduinoOTA.begin();
  ElegantOTA.begin(&OTAserver); // Start ElegantOTA
  OTAserver.begin();
  OTAserver.on("/", []()
               { OTAserver.send(200, "text/plain", "Hi! I am ESP8266."); });
}


void loop()
{
  ArduinoOTA.handle();
  OTAserver.handleClient();
  Blynk.run();
  timer.run();
  Blynk.virtualWrite(V4, "IP地址:", WiFi.localIP().toString());
  Blynk.virtualWrite(V5, "MAC地址:", WiFi.macAddress());
  Blynk.virtualWrite(V6, "RSSI:", WiFi.RSSI(), " ", "SSID: ", WiFi.SSID());
}