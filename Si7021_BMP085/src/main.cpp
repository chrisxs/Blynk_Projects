#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "OTA_setting.h"
#include <WebSerial.h>

#include <Adafruit_BMP085.h>
#include <BH1750FVI.h>
#include "Adafruit_Si7021.h"
#include <Wire.h>
#include <SPI.h>

void recvMsg(uint8_t *data, size_t len); // 声明一个函数 recvMsg，用于接收数据

AsyncWebServer serialserver(340); // 创建一个名为 serialserver 的异步 Web 服务器，端口为 340

Adafruit_Si7021 sensor = Adafruit_Si7021();
Adafruit_BMP085 bmp;

BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);

char auth[] = "你的Blynk_Token";
char ssid[] = "你的WiFi_SSID";
char pass[] = "你的WiFi密码";
char blynk_server[] = "你的Blynk服务器路径"; // Blynk服务器路径
int blynk_port = 8080;                       // Blynk端口号

BlynkTimer timer;

void sendSensor()
{
  float h = sensor.readHumidity();
  float t = sensor.readTemperature(); // dht.readTemperature(true)
  float p = bmp.readPressure();
  uint16_t l = LightSensor.GetLightIntensity();
  if (sensor.readHumidity() >= 100)
  {
    h = h + 100;
  }
  else if (sensor.readHumidity() <= 0)
  {
    h = h + 100;
  }
  else if (sensor.readHumidity() < 25)
  {
    h = h + 75;
  }
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, p / 100);
  Blynk.virtualWrite(V3, l);
}

void setup()
{
  Serial.begin(115200);
  WiFi.hostname("南室外");
  WebSerial.begin(&serialserver);
  WebSerial.msgCallback(recvMsg); // 附加反馈信息
  serialserver.begin();           // serialserver启动
  // Blynk.config(auth, IPAddress(xxx, xxx, xxx, xxx), 8080);
  Blynk.begin(auth, ssid, pass, blynk_server, blynk_port);
  LightSensor.begin();
  sensor.begin();
  bmp.begin();
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