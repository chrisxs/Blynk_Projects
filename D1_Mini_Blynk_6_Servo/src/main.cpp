#define BLYNK_PRINT Serial
#include <Arduino.h>
#include <FS.h> //this needs to be first, or it all crashes and burns...
#include <Servo.h>

#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson
#include <BlynkSimpleEsp8266.h>

#include <ArduinoOTA.h>

#include <Wire.h> // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"

#include <string>
#include <stdlib.h>

//define your default values here, if there are different values in config.json, they are overwritten.
std::string blynk_server;
std::string blynk_port;
std::string blynk_token;

Servo servo1, servo2, servo3, servo4, servo5, servo6; //声明舵机名称

//flag for saving data
bool shouldSaveConfig = false;

const int ResetButton = D5;
int ResetButtonState = digitalRead(ResetButton);

SSD1306Wire display(0x3c, D2, D1); // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h

//callback notifying us of the need to save config
void saveConfigCallback()
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

BLYNK_CONNECTED()
{
  Blynk.syncAll();
  //Blynk.syncVirtual(V1, V2, V3, V4, V5, V6);
}

BLYNK_WRITE(V1)
{
  servo1.write(param.asInt());
}

BLYNK_WRITE(V2)
{
  servo2.write(param.asInt());
}

BLYNK_WRITE(V3)
{
  servo3.write(param.asInt());
}

BLYNK_WRITE(V4)
{
  servo4.write(param.asInt());
}

BLYNK_WRITE(V5)
{
  servo5.write(param.asInt());
}

BLYNK_WRITE(V6)
{
  servo6.write(param.asInt());
}

void drawImageDemo()
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

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();

  pinMode(ResetButton, INPUT_PULLUP);

  servo1.attach(D0);
  servo2.attach(D3);
  servo3.attach(D4);
  servo4.attach(D6);
  servo5.attach(D7);
  servo6.attach(D8);

  servo1.write(90);
  servo2.write(90);
  servo3.write(90);
  servo4.write(90);
  servo5.write(90);
  servo6.write(90);

  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  //clean FS, for testing
  //SPIFFS.format();

  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin())
  {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json"))
    {
      //file exists, reading and loading
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
  //end read

  WiFiManagerParameter custom_blynk_server("server", "blynk server", blynk_server.c_str(), 40);
  WiFiManagerParameter custom_blynk_port("port", "blynk port", blynk_port.c_str(), 6);
  WiFiManagerParameter custom_blynk_token("blynk", "blynk token", blynk_token.c_str(), 32);
  WiFiManagerParameter custom_text("<p>点击SSID名称选择连接WiFi,并输入密码/服务器地址/设备口令</p>");

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //add all your parameters here
  wifiManager.addParameter(&custom_blynk_server);
  wifiManager.addParameter(&custom_blynk_port);
  wifiManager.addParameter(&custom_blynk_token);
  wifiManager.addParameter(&custom_text);

  //reset settings - for testing
  //wifiManager.resetSettings();

  if (ResetButtonState == LOW)
  {
    Serial.println("Getting Reset ESP Wifi-Setting.......");
    display.setFont(ArialMT_Plain_10);
    display.clear();
    display.drawString(0, 40, "RESET mode activated .");
    display.drawString(0, 50, "Please wait for reboot !");
    display.display();
    wifiManager.resetSettings();
    delay(5000);

    Serial.println("Formatting FS......");
    SPIFFS.format();

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

    display.clear();
    display.drawString(0, 40, "AP-SSID:RobotArm .");
    display.drawString(0, 50, "Password:none");
    display.display();

  if (!wifiManager.autoConnect("RobotArm", ""))
  {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  //read updated parameters
  blynk_server = custom_blynk_server.getValue();
  blynk_port = custom_blynk_port.getValue();
  blynk_token = custom_blynk_token.getValue();
  Serial.println("The values in the file are: ");
  Serial.println("\tblynk_server: " + String(custom_blynk_server.getValue()));
  Serial.println("\tblynk_port : " + String(custom_blynk_port.getValue()));
  Serial.println("\tblynk_token : " + String(custom_blynk_token.getValue()));

  //save the custom parameters to FS
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
    //end save
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());
  delay(500);
  ArduinoOTA.setHostname("RobotArm");
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

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
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
  Blynk.config(blynk_token.c_str(), blynk_server.c_str(), std::atoi(blynk_port.c_str()));
}

void loop()
{
  ArduinoOTA.handle();
  Blynk.run();
  drawImageDemo();
}