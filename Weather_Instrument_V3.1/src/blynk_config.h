#include <BlynkSimpleEsp8266.h>

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP085.h>
#include <WEMOS_SHT3X.h>
#include <BH1750FVI.h>

Adafruit_BMP085 bmp;
SHT3X sht30(0x44);
BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);

ESP8266WebServer blynk_config_server(8080);

String blynk_token;
String blynk_server;
int blynk_port;

BlynkTimer timer;

WidgetTerminal terminal(V7);

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
  Blynk.virtualWrite(V4, "IP地址 :", WiFi.localIP().toString());
  Blynk.virtualWrite(V5, "MAC地址  :", WiFi.macAddress());
  Blynk.virtualWrite(V6, "RSSI:", WiFi.RSSI(), " ", "SSID: ", WiFi.SSID());
}

BLYNK_WRITE(V7)
{
  if (String("blynk") == param.asStr())
  {
    terminal.println("Blynk Token: " + blynk_token);
    terminal.println("Blynk Server: " + blynk_server);
    terminal.println("Blyn Port: " + String(blynk_port));
  }
  if (String("wifi") == param.asStr())
  {
    terminal.println("SSID： " + WiFi.SSID());
    terminal.println("MAC： " + WiFi.macAddress());
    terminal.println("IP: " + WiFi.localIP().toString());
    // terminal.println("RSSI: " + WiFi.RSSI().toString());
    terminal.println("RSSI: " + String(WiFi.RSSI()));
  }
  if (String("reboot") == param.asStr())
  {
    ESP.restart();
  }
  if (String("clear") == param.asStr())
  {
    terminal.clear();
  }
  if (String("clear") == param.asStr())
  {
    terminal.clear();
  }

  // 确保所有内容都已发送
  terminal.flush();
}

void blynk_config_server_handleRoot()
{
  const char *http_username = "admin"; // 设置Blynk配置页面的账号名称
  const char *http_password = "admin"; // 设置Blynk配置页面的密码

  if (!blynk_config_server.authenticate(http_username, http_password))
  {
    blynk_config_server.requestAuthentication();
    return;
  }

  String html = R"HTML(
    <html>
    <head>
      <meta charset="UTF-8">
      <style> body { text-align: center; } </style>
    </head>
    <body>
      <h1>Blynk配置界面!</h1>
      <form action="/config" method="POST">
        <label for="token">Blynk Token</label><br>
        <input type="text" id="token" name="token" value="{TOKEN_VALUE}"><br>
        <label for="server">Blynk 服务器URL</label><br>
        <input type="text" id="server" name="server" value="{SERVER_VALUE}"><br>
        <label for="port">Blynk 端口号</label><br>
        <input type="text" id="port" name="port" value="{PORT_VALUE}"><br><br>
        <input type="submit" value="保存">
      </form><br>
      <form action="/clear" method="POST">
        <input type="submit" value="清除配置">
      </form>
    </body>
    </html>
  )HTML";

  html.replace("{TOKEN_VALUE}", blynk_token);
  html.replace("{SERVER_VALUE}", blynk_server);
  html.replace("{PORT_VALUE}", String(blynk_port));

  blynk_config_server.send(200, "text/html", html);
}

/*void blynk_config_server_handleRoot()
{
    blynk_config_server.send(200, "text/html", "<html><head>"
                                            "<meta charset=\"UTF-8\">"
                                            "<style> body { text-align: center; } </style>"
                                            "<form action='/config' method='POST'>"
                                            "<label for='token'>Blynk Token:</label><br>"
                                            "<input type='text' id='token' name='token' value='" + blynk_token + "'><br>"
                                            "<label for='server'>Blynk Server:</label><br>"
                                            "<input type='text' id='server' name='server' value='" + blynk_server + "'><br>"
                                            "<label for='port'>Blynk Port:</label><br>"
                                            "<input type='text' id='port' name='port' value='" + String(blynk_port) + "'><br><br>"
                                            "<input type='submit' value='Save'>"
                                            "</form><br>"
                                            "<form action='/clear' method='POST'>"
                                            "<input type='submit' value='Clear Configuration'>"
                                            "</form>");

}*/

/*void blynk_config_server_handleRoot()
{
    // 获取用户名和密码
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
    html += "<input type='submit' value='Save'>";
    html += "</form><br>";
    html += "<form action='/clear' method='POST'>";
    html += "<input type='submit' value='Clear Configuration'>";
    html += "</form>";
    html += "</body></html>";

    blynk_config_server.send(200, "text/html", html);
}*/

void blynk_config_server_handleConfig()
{
  if (blynk_config_server.method() == HTTP_POST)
  {
    blynk_token = blynk_config_server.arg("token");
    blynk_server = blynk_config_server.arg("server");
    blynk_port = blynk_config_server.arg("port").toInt();
    File configFile = SPIFFS.open("/blynk.txt", "w");
    if (!configFile)
    {
      Serial.println("文件写入失败");
      Serial.println();
    }
    configFile.printf("%s\n", blynk_token.c_str());
    configFile.printf("%s\n", blynk_server.c_str());
    configFile.printf("%d\n", blynk_port);
    configFile.close();
    blynk_config_server.send(200, "text/html", "<meta charset=\"UTF-8\"><p>Blynk配置保存成功！稍后即将重启！</p>");
    Serial.println("保存成功，你输入的是：");
    Serial.println(blynk_token);
    Serial.println(blynk_server);
    Serial.println(blynk_port);
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
    configFile.close();
    Serial.println("读取blynk.txt成功，文件中的内容如下:");
    Serial.println(blynk_token);
    Serial.println(blynk_server);
    Serial.println(String(blynk_port));
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
  Serial.println();
  // Blynk.config(blynk_token.c_str(), blynk_server.c_str(), blynk_port);

  // Blynk.config(blynk_token.c_str(), "chrisxs.com", blynk_port);

  // blynk_config_server.on("/", blynk_config_server_handleRoot);
  // blynk_config_server.on("/config", blynk_config_server_handleConfig);
  // blynk_config_server.on("/clear", blynk_config_server_handleClear);

  // blynk_config_server.begin();
  Serial.println();
}

void sensor_web_page()
{
  sht30.get();
  if (server.uri() != "/sensor_web_page")
  {
    server.send(404, "text/plain", "Not Found");
    return;
  }
  float h = sht30.humidity;
  float t = sht30.cTemp;
  float p = bmp.readPressure() / 100;
  uint16_t l = LightSensor.GetLightIntensity();

  String html = "<html><head><meta charset=\"UTF-8\">";
  html += "<style>";
  html += "html, body {";
  html += "height: 100%;";
  html += "display: flex;";
  html += "flex-direction: column;";
  html += "justify-content: center;";
  html += "align-items: center;";
  html += "}";
  html += "#chart_div_temperature, #chart_div_humidity {";
  html += "position: relative;";
  html += "left: 50px;"; // 调整仪表版的水平位置
  html += "}";
  html += "</style>";

  html += "<h1>4合1气象采集仪 V3.1</h1>";
  html += "<h2>Web诊断页面</h2>";

  // 默认是使用内置SPIFFS，使用前先输把loader.js刷入SPIFFS
  html += "<script type=\"text/javascript\" src=\"/loader.js\"></script>";

  // 使用Google Charts JS（偶尔会被BAN）
  html += "<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>";
  html += "<script type=\"text/javascript\">";
  html += "google.charts.load('current', {packages:['gauge']});";
  html += "google.charts.setOnLoadCallback(drawChart);";
  html += "function drawChart() {";
  html += "var dataTemp = google.visualization.arrayToDataTable([";
  html += "['Label', 'Value'],";
  html += "['温度', " + String(t) + "]";
  html += "]);";
  html += "var optionsTemp = {";
  html += "width: 1000, height: 120,";
  html += "redFrom: 35, redTo: 80,";
  html += "yellowFrom: 26, yellowTo: 35,";
  html += "minorTicks: 5,";
  html += "min: -20,";
  html += "max: 80";
  html += "};";
  html += "var formatterTemp = new google.visualization.NumberFormat({";
  html += "suffix: ' °C',";
  html += "fractionDigits: 1";
  html += "});";
  html += "formatterTemp.format(dataTemp, 1);";
  html += "var chartTemp = new google.visualization.Gauge(document.getElementById('chart_temp_div'));";
  html += "chartTemp.draw(dataTemp, optionsTemp);";

  html += "var dataHumidity = google.visualization.arrayToDataTable([";
  html += "['Label', 'Value'],";
  html += "['湿度', " + String(h) + "]";
  html += "]);";
  html += "var optionsHumidity = {";
  html += "width: 1000, height: 120,";
  html += "redFrom: 90, redTo: 100,";
  html += "yellowFrom: 75, yellowTo: 90,";
  html += "minorTicks: 5,";
  html += "max: 100"; // 设置湿度仪表盘的最大值为100
  html += "};";
  html += "var formatterHumidity = new google.visualization.NumberFormat({";
  html += "suffix: ' %',";
  html += "fractionDigits: 0";
  html += "});";
  html += "formatterHumidity.format(dataHumidity, 1);";
  html += "var chartHumidity = new google.visualization.Gauge(document.getElementById('chart_humidity_div'));";
  html += "chartHumidity.draw(dataHumidity, optionsHumidity);";

  html += "var dataPressure = google.visualization.arrayToDataTable([";
  html += "['Label', 'Value'],";
  html += "['气压', " + String(p) + "]";
  html += "]);";
  html += "var optionsPressure = {";
  html += "width: 1000, height: 120,";
  html += "redFrom: 1000, redTo: 1200,";
  html += "yellowFrom: 900, yellowTo: 1000,";
  html += "minorTicks: 5,";
  html += "min: 800,";
  html += "max: 1200";
  html += "};";
  html += "var formatterPressure = new google.visualization.NumberFormat({";
  html += "suffix: ' hPa',";
  html += "fractionDigits: 0";
  html += "});";
  html += "formatterPressure.format(dataPressure, 1);";
  html += "var chartPressure = new google.visualization.Gauge(document.getElementById('chart_pressure_div'));";
  html += "chartPressure.draw(dataPressure, optionsPressure);";

  html += "var dataLight = google.visualization.arrayToDataTable([";
  html += "['Label', 'Value'],";
  html += "['光照强度', " + String(l) + "]";
  html += "]);";
  html += "var optionsLight = {";
  html += "width: 1000, height: 120,";
  html += "redFrom: 800, redTo: 1000,";
  html += "yellowFrom: 500, yellowTo: 800,";
  html += "minorTicks: 5,";
  html += "min: 0,";
  html += "max: 1000";
  html += "};";
  html += "var formatterLight = new google.visualization.NumberFormat({";
  html += "suffix: ' lx',";
  html += "fractionDigits: 0";
  html += "});";
  html += "formatterLight.format(dataLight, 1);";
  html += "var chartLight = new google.visualization.Gauge(document.getElementById('chart_light_div'));";
  html += "chartLight.draw(dataLight, optionsLight);";

  html += "}";
  html += "</script>";
  html += "</head><body>";
  html += "<h3>温度和湿度</h3>";
  html += "<div style=\"display: flex;\">"; // 使用 flexbox 布局
  html += "<div id=\"chart_temp_div\" style=\"margin-right: 30px;\"></div>";
  html += "<div id=\"chart_humidity_div\" style=\"margin-right: 30px;\"></div>";
  html += "<div id=\"chart_pressure_div\" style=\"margin-right: 30px;\"></div>";
  html += "<div id=\"chart_light_div\" style=\"margin-right: 30px;\"></div>";
  html += "</div>";
  html += "<p style=\"line-height: 0;\">温度：" + String(t, 1) + " \u00B0C</p>";
  html += "<p style=\"line-height: 0;\">湿度：" + String(h, 0) + " %</p>";
  html += "<p style=\"line-height: 0;\">大气压：" + String(p, 1) + " hPa</p>";
  html += "<p style=\"line-height: 0;\">光照度：" + String(l) + " lux</p>";
  html += "<p style=\"line-height: 0;\">WiFi SSID：" + WiFi.SSID() + "</p>";
  html += "<p style=\"line-height: 0;\">IP地址：" + WiFi.localIP().toString() + "</p>";
  html += "<p style=\"line-height: 0;\">MCA地址：" + WiFi.macAddress() + "</p>";
  html += "<p style=\"line-height: 0;\">RSSI：" + String(WiFi.RSSI()) + " dB</p>";

  html += "</body></html>";

  server.send(200, "text/html", html);
}

void serveChartsLoader()
{
  File file = SPIFFS.open("/loader.js", "r");
  if (!file)
  {
    server.send(404, "text/plain", "File not found");
    return;
  }
  server.streamFile(file, "text/javascript");
  file.close();
}