#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


#include <Adafruit_Si7021.h>
#include <MAX44009.h>
#include <BMx280I2C.h>
#include <Wire.h>
#include <SPI.h>

#define I2C_ADDRESS 0x76

BMx280I2C bmx280(I2C_ADDRESS);

MAX44009 light;

Adafruit_Si7021 sensor = Adafruit_Si7021();

char auth[] = "73e65c339daa43dcb4ac653080dabac5";
char ssid[] = "chrisxs_01";
char pass[] = "aj20160223";

BlynkTimer timer;

void sendSensor()
{
  if (!bmx280.measure())
  {
    Serial.println("could not start measurement, is a measurement already running?");
    return;
  }

  //wait for the measurement to finish
  do
  {
    delay(100);
  } while (!bmx280.hasValue());

  Serial.print("Pressure: "); Serial.println(bmx280.getPressure());
  Serial.print("Temperature: "); Serial.println(bmx280.getTemperature());

  if (bmx280.isBME280())
  {
    Serial.print("Humidity: ");
    Serial.println(bmx280.getHumidity());
  }

  float h = sensor.readHumidity();
  float t = sensor.readTemperature(); //dht.readTemperature(true)
  float p = bmx280.getPressure();
  float l = light.get_lux();
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, p / 100);
  Blynk.virtualWrite(V3, l);
}

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  if (!bmx280.begin())
  {
    Serial.println("begin() failed. check your BMx280 Interface and I2C Address.");
    while (1);
  }

  if (bmx280.isBME280())
    Serial.println("sensor is a BME280");
  else
    Serial.println("sensor is a BMP280");

  //reset sensor to default parameters.
  bmx280.resetToDefaults();

  //by default sensing is disabled and must be enabled by setting a non-zero
  //oversampling setting.
  //set an oversampling setting for pressure and temperature measurements.
  bmx280.writeOversamplingPressure(BMx280MI::OSRS_P_x16);
  bmx280.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);

  //if sensor is a BME280, set an oversampling setting for humidity measurements.
  if (bmx280.isBME280())
    bmx280.writeOversamplingHumidity(BMx280MI::OSRS_H_x16);

  //如果不适用WiFimanager,就必须使用WiFi.begin()函数来连接WiFi,不可以用blynk.begin,否则更改token后会导致无法OTA
  WiFi.begin(ssid, pass);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.setHostname("北面阳台室外-V2");
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
  sensor.begin();
  light.begin();
  Blynk.config(auth,IPAddress(121,4,166,72), 8080);
  //Blynk.begin(auth, ssid, pass, IPAddress(121,40,178,75), 8080);
  timer.setInterval(1000L, sendSensor);
}


void loop()
{
  ArduinoOTA.handle();
  Blynk.run();
  timer.run();
  Blynk.virtualWrite(V4, "IP地址 :", WiFi.localIP().toString());
  Blynk.virtualWrite(V5, "MAC地址  :", WiFi.macAddress());
  Blynk.virtualWrite(V6, "RSSI:", WiFi.RSSI(), " ", "SSID: ", WiFi.SSID());
}
