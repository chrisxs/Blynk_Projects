#define BLYNK_PRINT Serial
#include <SPIFFS.h>
#include <FS.h>          // this needs to be first, or it all crashes and burns...
#include <Arduino.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

#include <BlynkSimpleEsp32.h>

//#include "WebPage.h"

#include <string>
#include <stdlib.h>

//用于WiFiManager界面中的变量服务器域名、端口、口令
std::string blynk_server;
std::string blynk_port;
std::string blynk_token;

//标记是否储存
bool shouldSaveConfig = false;

//WebServer server(80);
//const char* host = "esp32";

/////D5引脚用作WiFi复位按钮，D4引脚用作LED指示灯/////
const int ResetButton = 33;
const int LED = 26;


//回调通知我们需要保存配置
void saveConfigCallback()
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void setup(void)
{
  // put your setup code here, to run once:
  int ResetButtonState = digitalRead(ResetButton);
  Serial.begin(115200);
  Serial.println();
  digitalWrite(LED, LOW);
  pinMode(ResetButton, INPUT_PULLUP);
  pinMode(LED, OUTPUT);

  //////////wifimanager//////////

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
  //SPIFFS.format();

  if (ResetButtonState == HIGH)
  {
    Serial.println("Getting Reset ESP Wifi-Setting.......");
    digitalWrite(LED, LOW);
    wifiManager.resetSettings();
    digitalWrite(LED, HIGH);
    delay(5000);
    digitalWrite(LED, LOW);

    Serial.println("Formatting FS......");
    digitalWrite(LED, HIGH);
    SPIFFS.format();
    delay(5000);
    digitalWrite(LED, LOW);

    Serial.println("Done Reboot In 5 seconds");
    delay(5000);
    ESP.restart();
  }

 
  if (!wifiManager.autoConnect("ESP32-WM-OTA-DEMO", ""))
  {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  //read updated parameters
  blynk_server = custom_blynk_server.getValue();
  blynk_port = custom_blynk_port.getValue();
  blynk_token = custom_blynk_token.getValue();

  //save the custom parameters to FS
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
  Serial.println(blynk_token.c_str());


/*use mdns for host name resolution
  if (!MDNS.begin(host))
  { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1)
    {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  //return index page which is stored in serverIndex//
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  //handling uploading firmware file
  server.on(
      "/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart(); }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      //flashing firmware to ESP//
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    } });
  server.begin();*/
  Blynk.config(blynk_token.c_str(), blynk_server.c_str(), std::atoi(blynk_port.c_str()));
}

void loop(void)
{
  //server.handleClient();
  Blynk.run();
}