#include <Arduino.h>
#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino 不能使用D1minipro,会死机

//////////wifimanager//////////
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager 0.15.0
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson 5.13.5

//////////OTA//////////
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//////////Blynk////////
#include <BlynkSimpleEsp8266.h>

//////////Si7021//////////
#include "Adafruit_Si7021.h"

#include <string>
#include <stdlib.h>


//define your default values here, if there are different values in config.json, they are overwritten.
std::string blynk_server;
std::string blynk_port ;
std::string blynk_token;

//flag for saving data
bool shouldSaveConfig = false;
const int ResetButton = D5;
const int LedPin = D4;
int ResetButtonState = digitalRead(ResetButton);

Adafruit_Si7021 sensor = Adafruit_Si7021();
BlynkTimer timer;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}


void sendSensor()
{
  float h = sensor.readHumidity();
  float t = sensor.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
  Blynk.virtualWrite(V0, h);
  Blynk.virtualWrite(V1, t);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  digitalWrite(LedPin, HIGH);
  pinMode(ResetButton, INPUT_PULLUP);
  pinMode(LedPin, OUTPUT);

  //clean FS, for testing
  //SPIFFS.format();

  //////////wifimanager//////////

  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");

          blynk_server = json["blynk_server"].as<const char *>();
          blynk_port = json["blynk_port"].as<const char *>();
          blynk_token = json["blynk_token"].as<const char *>();

        } else {
          Serial.println("failed to load json config");
        }
        configFile.close();
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_blynk_server("server", "blynk server", blynk_server.c_str(), 40);
  WiFiManagerParameter custom_blynk_port("port", "blynk port", blynk_port.c_str(), 6);
  WiFiManagerParameter custom_blynk_token("blynk", "blynk token", blynk_token.c_str(), 32);
  WiFiManagerParameter custom_text("<p>点击SSID名称选择连接WiFi,并输入密码/服务器地址/设备口令</p>");

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //set static ip
  //wifiManager.setSTAStaticIPConfig(IPAddress(10, 0, 1, 99), IPAddress(10, 0, 1, 1), IPAddress(255, 255, 255, 0));

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
    digitalWrite(LedPin, LOW);
    delay(1000);
    digitalWrite(LedPin, HIGH);
    delay(300);
    digitalWrite(LedPin, LOW);
    delay(1000);
    digitalWrite(LedPin, HIGH);
    delay(100);
    digitalWrite(LedPin, LOW);
    delay(1000);
    digitalWrite(LedPin, HIGH);

    wifiManager.resetSettings();
    delay(10000);

    digitalWrite(LedPin, LOW);
    delay(1000);
    digitalWrite(LedPin, HIGH);
    delay(300);
    digitalWrite(LedPin, LOW);
    delay(1000);
    digitalWrite(LedPin, HIGH);
    delay(100);
    digitalWrite(LedPin, LOW);
    delay(1000);
    digitalWrite(LedPin, HIGH);
    Serial.println("Formatting FS......");
    SPIFFS.format();
    delay(10000);

    Serial.println("Done Reboot In 5 seconds");
    digitalWrite(LedPin, LOW);
    delay(5000);
    digitalWrite(LedPin, HIGH);
    ESP.restart();
  }

  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%
  //wifiManager.setMinimumSignalQuality();

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  //wifiManager.setTimeout(120);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("Si7021-DEMO", "")) {
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

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["blynk_server"] = blynk_server.c_str();
    json["blynk_port"] = blynk_port.c_str();
    json["blynk_token"] = blynk_token.c_str();

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());
  Serial.println(blynk_token.c_str());

  //////////////////////////////OTA//////////////////////

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("Si7021-2B33");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
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
    }
  });
  ArduinoOTA.begin();
  sensor.begin();
  Serial.println( WiFi.localIP().toString());
  Serial.println(blynk_server.c_str());
  Serial.println(blynk_port.c_str());
  Serial.println(blynk_token.c_str());
  Blynk.config(blynk_token.c_str(), blynk_server.c_str(), std::atoi(blynk_port.c_str()));
  timer.setInterval(1000L, sendSensor);

}

void loop() {
  // put your main code here, to run repeatedly:
  ArduinoOTA.handle();
  Blynk.run();
  timer.run();
  Blynk.virtualWrite(V2, "IP地址:", WiFi.localIP().toString());
  Blynk.virtualWrite(V3, "MAC地址:", WiFi.macAddress());
  Blynk.virtualWrite(V4, "RSSI:", WiFi.RSSI(), " ", "SSID: ", WiFi.SSID());
}