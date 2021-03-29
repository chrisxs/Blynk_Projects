#include <Arduino.h>

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

const int MosPin = 15;

char auth[] = "你的TOKEN";
char ssid[] = "你的SSID";
char pass[] = "你的WiFi密码";

/////磁力锁/////
BLYNK_WRITE(V0) {
  //int buttonState = param.asInt();
  if (param.asInt() == 1) {
    // turn LED on:
    digitalWrite(MosPin, LOW);
    delay(1000);
    digitalWrite(MosPin, HIGH);
  }
}

/////电插锁/////
/*BLYNK_WRITE(V0) {
  //int buttonState = param.asInt();
  if (param.asInt() == 1) {
    // turn LED on:
    digitalWrite(MosPin, HIGH);
    delay(5000);
    digitalWrite(MosPin, LOW);
  }
  }*/

/////电插锁为LOW/////
BLYNK_CONNECTED() {
  digitalWrite(MosPin, HIGH);
  digitalWrite(2, LOW);
}

/*
/////电插锁为LOW/////
BLYNK_APP_CONNECTED() {
  digitalWrite(MosPin, HIGH); 
}

/////电插锁为LOW/////
BLYNK_APP_DISCONNECTED() {
  digitalWrite(MosPin, HIGH);
}*/

void setup() {
  pinMode(MosPin, OUTPUT);
  digitalWrite(MosPin, HIGH);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  Serial.begin(115200);

  //如果不适用WiFimanager,就必须使用WiFi.begin()函数来连接WiFi,不可以用blynk.begin,否则更改token后会导致无法OTA
  WiFi.begin(ssid, pass);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("Blynk_DoorLocker-DEMO");
  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

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
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //delay(5000);
  Blynk.config(auth, "你的域名", 80);  //Blynk.config(auth, "你的域名", 你的端口);
}

void loop() {
  ArduinoOTA.handle();
  Blynk.run();
}