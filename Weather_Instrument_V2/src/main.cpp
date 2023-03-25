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

#include "OTA_setting.h"
void recvMsg(uint8_t *data, size_t len); // 声明一个函数 recvMsg，用于接收数据

AsyncWebServer serialserver(340); // 创建一个名为 serialserver 的异步 Web 服务器，端口为 340

#define I2C_ADDRESS 0x76

BMx280I2C bmx280(I2C_ADDRESS);

MAX44009 light;

Adafruit_Si7021 sensor = Adafruit_Si7021();

char auth[] = "你的Blynk_Token";
char ssid[] = "你的WiFi_SSID";
char pass[] = "你的WiFi密码";
char blynk_server[] = "你的Blynk服务器路径"; // Blynk服务器路径
int blynk_port = 8080;               // Blynk端口号

BlynkTimer timer;

void sendSensor()
{
  if (!bmx280.measure())
  {
    Serial.println("could not start measurement, is a measurement already running?");
    return;
  }

  // wait for the measurement to finish
  do
  {
    delay(100);
  } while (!bmx280.hasValue());

  Serial.print("Pressure: ");
  Serial.println(bmx280.getPressure());
  Serial.print("Temperature: ");
  Serial.println(bmx280.getTemperature());

  if (bmx280.isBME280())
  {
    Serial.print("Humidity: ");
    Serial.println(bmx280.getHumidity());
  }

  float h = sensor.readHumidity();
  float t = sensor.readTemperature(); // dht.readTemperature(true)
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
  WebSerial.begin(&serialserver);
  WebSerial.msgCallback(recvMsg); // 附加反馈信息
  serialserver.begin();           // serialserver启动
  WiFi.hostname("北室外");
  Wire.begin();
  if (!bmx280.begin())
  {
    Serial.println("begin() failed. check your BMx280 Interface and I2C Address.");
    while (1)
      ;
  }

  if (bmx280.isBME280())
    Serial.println("sensor is a BME280");
  else
    Serial.println("sensor is a BMP280");

  // reset sensor to default parameters.
  bmx280.resetToDefaults();

  // by default sensing is disabled and must be enabled by setting a non-zero
  // oversampling setting.
  // set an oversampling setting for pressure and temperature measurements.
  bmx280.writeOversamplingPressure(BMx280MI::OSRS_P_x16);
  bmx280.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);

  // if sensor is a BME280, set an oversampling setting for humidity measurements.
  if (bmx280.isBME280())
    bmx280.writeOversamplingHumidity(BMx280MI::OSRS_H_x16);

  // 如果不适用WiFimanager,就必须使用WiFi.begin()函数来连接WiFi,不可以用blynk.begin,否则更改token后会导致无法OTA
  WiFi.begin(ssid, pass);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  sensor.begin();
  light.begin();
  // Blynk.config(auth, IPAddress(xxx, xxx, xxx, xxx), 8080);
  Blynk.begin(auth, ssid, pass, blynk_server, blynk_port);
  timer.setInterval(1000L, sendSensor);
  OTA();
}

void loop()
{
  Blynk.run();
  timer.run();
  Blynk.virtualWrite(V4, "IP地址 :", WiFi.localIP().toString());
  Blynk.virtualWrite(V5, "MAC地址  :", WiFi.macAddress());
  Blynk.virtualWrite(V6, "RSSI:", WiFi.RSSI(), " ", "SSID: ", WiFi.SSID());
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
    WebSerial.println("blynk token: " + String(auth) + "\nssid: " + String(ssid) + "\nip: " + String(WiFi.localIP().toString()) + "\nblynk_server: " + String(blynk_server) + "\nblynk_port: " + String(blynk_port) + "\nhostname: " + String(WiFi.hostname()));
    Serial.println("\n收到debug命令\n");
    Serial.println("blynk token: " + String(auth) + "\nssid: " + String(ssid) + "\nip: " + String(WiFi.localIP().toString()) + "\nblynk_server: " + String(blynk_server) + "\nblynk_port: " + String(blynk_port) + "\nhostname: " + String(WiFi.hostname()));
  }
}