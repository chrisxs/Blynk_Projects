#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#include <Arduino.h>
#include <FS.h> //如果不正常就放到第一位

#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

#include <DNSServer.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson

#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <WEMOS_SHT3X.h>
#include <BH1750FVI.h>

#include <string>
#include <stdlib.h>
#include <SPI.h>

Adafruit_BMP085 bmp;
SHT3X sht30(0x44);
BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);

const int ResetButton = D0;
const int LedPin = D4;
int ResetButtonState = digitalRead(ResetButton);
//int ResetButtonState = 0;

//define your default values here, if there are different values in config.json, they are overwritten.
std::string blynk_server;
std::string blynk_port;
std::string blynk_token;

//用于保存数据的标志
bool shouldSaveConfig = false;
BlynkTimer timer;

//回调通知我们需要保存配置
void saveConfigCallback()
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void sendSensor()
{
  sht30.get();
  uint16_t lux = LightSensor.GetLightIntensity();
  float h = sht30.humidity;
  float t = sht30.cTemp;
  float p = bmp.readPressure();
  float l = lux;
  Blynk.virtualWrite(V0, h);
  Blynk.virtualWrite(V1, t);
  Blynk.virtualWrite(V2, p / 100);
  Blynk.virtualWrite(V3, l);
}

void setup()
{
  Serial.begin(115200);

  bmp.begin();
  LightSensor.begin();

  digitalWrite(LedPin, HIGH);
  pinMode(ResetButton, INPUT);
  pinMode(LedPin, OUTPUT);

  //格式化 FS, 测试用
  //SPIFFS.format();

  //读取FS json的配置
  Serial.println("mounting FS...");
  if (SPIFFS.begin())
  {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json"))
    {
      //文件存在，读取和加载
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile)
      {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // 分配缓冲区以存储文件的内容。
        std::unique_ptr<char[]> buf(new char[size]);
        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject &json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success())
        {
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
  //结束

  // 要配置的额外参数（可以是全局的，也可以只是在设置中）
  // 连接后，parameter.getValue（）将获得配置的值//After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt 的默认长度
  WiFiManagerParameter custom_blynk_server("server", "blynk server", blynk_server.c_str(), 40);
  WiFiManagerParameter custom_blynk_port("port", "blynk port", blynk_port.c_str(), 6);
  WiFiManagerParameter custom_blynk_token("blynk", "blynk token", blynk_token.c_str(), 32);
  WiFiManagerParameter custom_text("<p>点击SSID名称选择连接WiFi,并输入密码/服务器地址/设备口令</p>");

  //WiFiManager
  //本地初始化。一旦业务完成，就没有必要保留它
  WiFiManager wifiManager;

  //设置配置保存通知回调
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //设置静态IP
  //wifiManager.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  //在这里添加所有参数
  wifiManager.addParameter(&custom_blynk_server);
  wifiManager.addParameter(&custom_blynk_port);
  wifiManager.addParameter(&custom_blynk_token);
  wifiManager.addParameter(&custom_text);

  //重置设置 - 用于测试
  //wifiManager.resetSettings();

  //设置重置按钮的功能
  if (ResetButtonState == LOW)
  {
    Serial.println("Getting Reset ESP Wifi-Setting.......");
    digitalWrite(LedPin, LOW);
    delay(3000);
    digitalWrite(LedPin, HIGH);
    delay(300);
    digitalWrite(LedPin, LOW);
    delay(3000);
    digitalWrite(LedPin, HIGH);
    delay(300);
    digitalWrite(LedPin, LOW);
    delay(3000);
    digitalWrite(LedPin, HIGH);

    wifiManager.resetSettings();
    delay(10000);

    Serial.println("Formatting FS......");
    SPIFFS.format();

    delay(10000);
    Serial.println("Done Reboot In 5 seconds");
    digitalWrite(LedPin, LOW);
    delay(5000);
    digitalWrite(LedPin, HIGH);
    ESP.restart();
  }

  //设置最低信号质量,如果信号质量低于8%,则略这些AP
  //默认值为:8%
  //wifiManager.setMinimumSignalQuality();

  //设置超时直到配置关闭
  //有用的是让它全部重试或进入睡眠状态
  //wifiManager.setTimeout(120);

  //获取ssid并传递并尝试连接
  //如果它没有连接，它将启动具有指定名称的访问点,"AutoConnectAP",并进入等待配置的阻塞循环
  if (!wifiManager.autoConnect("4合1气象采集仪V3-DEMO", ""))
  {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //重置并重试，或者让它深入睡眠
    ESP.reset();
    delay(5000);
  }

  //如果已经连接
  Serial.println("connected...yeey :)");
  //读取更新的参数
  blynk_server = custom_blynk_server.getValue();
  blynk_port = custom_blynk_port.getValue();
  blynk_token = custom_blynk_token.getValue();
  //j将自定义参数保存到FS
  if (shouldSaveConfig)
  {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    json["blynk_server"] = blynk_server.c_str();
    json["blynk_port"] = blynk_port.c_str();
    json["blynk_token"] = blynk_token.c_str();
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile)
    {
      Serial.println("failed to open config file for writing");
    }
    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }
  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  //////////////////////////////////////////////////////////////////////////////////////////OTA部分///////////////////////////////////////////////

  // 默认端口为"8266"
  // ArduinoOTA.setPort(8266);

  // 默认主机名为:esp8266-[ChipID]
  ArduinoOTA.setHostname("4合1气象采集仪V3-29c5");

  // 默认情况下不验证
  // ArduinoOTA.setPassword("admin");
  // 密码也可以用它的md5值设置
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
    {
      type = "sketch";
    }
    else
    { // U_SPIFFS
      type = "filesystem";
    }
    //注意：如果更新SPIFFS，这将是使用SPIFFS.end（）卸载SPIFFS的地方
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
    {
      Serial.println("Auth Failed");
    }
    else if (error == OTA_BEGIN_ERROR)
    {
      Serial.println("Begin Failed");
    }
    else if (error == OTA_CONNECT_ERROR)
    {
      Serial.println("Connect Failed");
    }
    else if (error == OTA_RECEIVE_ERROR)
    {
      Serial.println("Receive Failed");
    }
    else if (error == OTA_END_ERROR)
    {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP().toString());
  Serial.println(blynk_server.c_str());
  Serial.println(blynk_port.c_str());
  Serial.println(blynk_token.c_str());
  Blynk.config(blynk_token.c_str(), blynk_server.c_str(), std::atoi(blynk_port.c_str()));
  timer.setInterval(1000L, sendSensor);
}
void loop()
{
  ArduinoOTA.handle();
  Blynk.run();
  timer.run();
  Blynk.virtualWrite(V4, "IP地址:", WiFi.localIP().toString());
  Blynk.virtualWrite(V5, "MAC地址:", WiFi.macAddress());
  Blynk.virtualWrite(V6, "RSSI:", WiFi.RSSI(), " ", "SSID: ", WiFi.SSID());
}