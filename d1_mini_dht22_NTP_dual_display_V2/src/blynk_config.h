#include <BlynkSimpleEsp8266.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

ESP8266WebServer blynk_config_server(8080);

// 用于储存Blynk的值
String blynk_token;
String blynk_server;
int blynk_port;

char ntp_server[] = "ntp.aliyun.com"; // NTP服务器

BlynkTimer timer;
WidgetTerminal terminal(V10);

DHT dht(D2, DHT22);

void sendSensor()
{
  float h = dht.readHumidity();    // 读取湿度值
  float t = dht.readTemperature(); // 读取温度值
  Blynk.virtualWrite(V0, t);       // 将温度值写入虚拟引脚 V0
  Blynk.virtualWrite(V1, h);       // 将湿度值写入虚拟引脚 V1

  Blynk.virtualWrite(V2, "IP地址: ", WiFi.localIP().toString());             // 在Blynk app上显示本地IP地址
  Blynk.virtualWrite(V3, "MAC地址: ", WiFi.macAddress());                    // 在Blynk app上显示MAC地址
  Blynk.virtualWrite(V4, "RSSI: ", WiFi.RSSI(), " ", "SSID: ", WiFi.SSID()); // 在Blynk app上显示WiFi信号强度和SSID
}

// 用于诊断用的Blynk终端插件，不需要可以注释掉或者删掉
BLYNK_WRITE(V10)
{
  if (String("blynk") == param.asStr()) // 输入blynk显示Blynk配置信息
  {
    terminal.println("Blynk Token: " + blynk_token);
    terminal.println("Blynk Server: " + blynk_server);
    terminal.println("Blyn Port: " + String(blynk_port));
  }
  if (String("wifi") == param.asStr()) // 输入WiFi显示WiFi配置信息
  {
    terminal.println("SSID： " + WiFi.SSID());
    terminal.println("MAC： " + WiFi.macAddress());
    terminal.println("IP: " + WiFi.localIP().toString());
    terminal.println("RSSI: " + String(WiFi.RSSI()));
    terminal.println("NTP 服务器: " + String(ntp_server));
  }
  if (String("reboot") == param.asStr()) // 输入reboot重启ESP8266
  {
    ESP.restart();
  }
  if (String("clear") == param.asStr()) // 输入clear清屏
  {
    terminal.clear();
  }
  terminal.flush();
}

BLYNK_CONNECTED()
{
  Blynk.syncVirtual(V5); // 在连接 Blynk 服务器后同步虚拟引脚 V5
  terminal.clear();      // 连接Blynk后清屏
}

void blynk_config_server_handleRoot()
{ // 获取用户名和密码
  const char *http_username = "admin";
  const char *http_password = "admin";

  // 检查是否提供了用户名和密码
  if (!blynk_config_server.authenticate(http_username, http_password))
  {
    // 如果没有提供用户名和密码，则发送身份验证失败的响应
    blynk_config_server.requestAuthentication();
    return;
  }
  String html = "<html><head><meta charset=\"UTF-8\"><style> body { text-align: center; } </style></head><body>";
  html += "<h1>Blynk配置界面!</h1>";
  html += "<form action='/config' method='POST'>";
  html += "<label for='token'>Blynk Token</label><br>";
  html += "<input type='text' id='token' name='token' value='" + blynk_token + "'><br>";
  html += "<label for='server'>Blynk 服务器URL</label><br>";
  html += "<input type='text' id='server' name='server' value='" + blynk_server + "'><br>";
  html += "<label for='port'>Blynk 端口号</label><br>";
  html += "<input type='text' id='port' name='port' value='" + String(blynk_port) + "'><br><br>";
  // html += "<label for='ntp_server'>NTP 服务器</label><br>";
  // html += "<input type='text' id='ntp_server' name='ntp_server' value='" + ntp_server + "'><br><br>";
  html += "<input type='submit' value='保存'>";
  html += "</form><br>";
  html += "<form action='/clear' method='POST'>";
  html += "<input type='submit' value='清除配置'>";
  html += "</form>";
  html += "</body></html>";

  blynk_config_server.send(200, "text/html", html);
}

void blynk_config_server_handleConfig()
{
  if (blynk_config_server.method() == HTTP_POST)
  {
    blynk_token = blynk_config_server.arg("token");
    blynk_server = blynk_config_server.arg("server");
    blynk_port = blynk_config_server.arg("port").toInt();
    // ntp_server = blynk_config_server.arg("ntp_server");
    File configFile = SPIFFS.open("/blynk.txt", "w");
    if (!configFile)
    {
      Serial.println("文件写入失败");
      Serial.println();
    }
    configFile.printf("%s\n", blynk_token.c_str());
    configFile.printf("%s\n", blynk_server.c_str());
    configFile.printf("%d\n", blynk_port);
    // configFile.printf("%d\n", ntp_server.c_str());
    configFile.close();
    blynk_config_server.send(200, "text/html", "<meta charset=\"UTF-8\"><p>Blynk配置保存成功！稍后即将重启！</p>");
    Serial.println("保存成功，你输入的是：");
    Serial.println(blynk_token);
    Serial.println(blynk_server);
    Serial.println(blynk_port);
    // Serial.println(ntp_server);
    Serial.println("设备即将重启");
    Serial.println();
    delay(3000);
    ESP.restart();
  }
  else
  {
    Serial.println("保存失败，你输入的是：");
    Serial.println(blynk_token);
    Serial.println(blynk_server);
    Serial.println(blynk_port);
    // Serial.println(ntp_server);
    Serial.println();
    blynk_config_server.send(400, "text/plain", "Invalid Request");
  }
}

void blynk_config_server_handleClear()
{
  if (blynk_config_server.method() == HTTP_POST)
  {
    SPIFFS.remove("/blynk.txt");
    blynk_config_server.send(200, "text/html", "<meta charset=\"UTF-8\"><p>Blynk配置已经清除！稍后将会重启！</p>");
    Serial.println("Blynk配置已经清除！稍后将会重启！");
    Serial.println();
    delay(3000);
    ESP.restart();
  }
  else
  {
    blynk_config_server.send(400, "text/plain", "Invalid Request");
  }
}

void load_blynk_config()
{
  SPIFFS.begin();
  File configFile = SPIFFS.open("/blynk.txt", "r");
  if (configFile)
  {
    blynk_token = configFile.readStringUntil('\n');
    // blynk_token.trim();
    blynk_server = configFile.readStringUntil('\n');
    // blynk_server.trim();
    blynk_port = configFile.readStringUntil('\n').toInt();
    // ntp_server = configFile.readStringUntil('\n');
    configFile.close();
    Serial.println("读取blynk.txt成功，文件中的内容如下:");
    Serial.println(blynk_token);
    Serial.println(blynk_server);
    Serial.println(String(blynk_port));
    // Serial.println(ntp_server);
    Serial.println("Blynk配置服务启动成功");
    Serial.println();
  }
  else
  {
    Serial.println("当前设备blynk.txt不存在，无法提取内容\n");
  }
  Serial.println("当前Blynk连接参数如下：");
  Serial.println("Blynk Token: " + blynk_token);
  Serial.println("Blynk Server: " + blynk_server);
  Serial.println("Blyn Port: " + String(blynk_port));
  // Serial.println("NTP Server: " + ntp_server);
  Serial.println();
}