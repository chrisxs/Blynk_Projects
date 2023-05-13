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

bool autoRefresh = true; // 默认为开启自动刷新

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
{ // 输入blynk显示Blynk配置信息
  if (String("blynk") == param.asStr())
  {
    terminal.println("Blynk Token: " + blynk_token);
    terminal.println("Blynk Server: " + blynk_server);
    terminal.println("Blyn Port: " + String(blynk_port));
  }

  // 输入WiFi显示WiFi配置信息
  if (String("wifi") == param.asStr())
  {
    terminal.println("WiFi SSID： " + WiFi.SSID());
    terminal.println("MAC地址： " + WiFi.macAddress());
    terminal.println("IP地址： " + WiFi.localIP().toString());
    terminal.println("RSSI：" + String(WiFi.RSSI()));
    terminal.println("NTP服务器：" + String(ntp_server));
  }

  // 输入reboot重启ESP8266
  if (String("reboot") == param.asStr())
  {
    ESP.restart();
  }

  // 输入clear清屏
  if (String("clear") == param.asStr())
  {
    terminal.clear();
  }

  // 输入js检查文件是否存在
  if (String("js") == param.asStr())
  {
    if (SPIFFS.exists("/loader.js"))
    {
      File file = SPIFFS.open("/loader.js", "r");
      if (file)
      {
        String filePath = file.name();
        terminal.println("loader.js文件存在，路径为: " + filePath);
        file.close();
      }
      else
      {
        terminal.println("打开loader.js文件失败！或文件不存在！");
      }
    }
  }

  // 列出所有 SPIFFS 文件
  if (String("list") == param.asStr())
  {
    terminal.println("当前SPIFFS存在下列文件：");
    Dir dir = SPIFFS.openDir("/");
    while (dir.next())
    {
      String fileName = dir.fileName();
      terminal.println(fileName);
    }
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
    Serial.println("Blynk配置读取成功");
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

/*void dht22() //纯文本显示页面
{
  if (server.uri() != "/dht22")
  {
    server.send(404, "text/plain", "Not Found");
    return;
  }
  String html = "<html><head><meta charset=\"UTF-8\"></head><body>";  // 设置网页编码为UTF-8
  html += "<h2>温度和湿度</h2>";  // 使用中文字符
  html += "<p>温度：" + String(dht.readTemperature()) + "\u00B0C</p>";
  html += "<p>湿度：" + String(dht.readHumidity()) + "%</p>";
  html += "<p>WiFi SSID：" + WiFi.SSID() + "</p>";
  html += "<p>IP地址：" + WiFi.localIP().toString() + "</p>";
  html += "<p>RSSI：" + String(WiFi.RSSI()) + "db</p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}
*/

void dht22()
{
  if (server.uri() != "/dht22")
  {
    server.send(404, "text/plain", "Not Found");
    return;
  }
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

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
  html += "<h2>Blynk版本DHT22双显示屏小时钟WEB诊断页面</h2>";

  // 默认是使用内置SPIFFS，使用前先输把loader.js刷入SPIFFS
  html += "<script type=\"text/javascript\" src=\"/loader.js\"></script>";

  // 使用Google Charts JS（偶尔会被BAN）
  // html += "<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>";
  html += "<script type=\"text/javascript\">";
  html += "google.charts.load('current', {packages:['gauge']});";
  html += "google.charts.setOnLoadCallback(drawChart);";
  html += "function drawChart() {";
  html += "var dataTemp = google.visualization.arrayToDataTable([";
  html += "['Label', 'Value'],";
  html += "['温度', " + String(temperature) + "]";
  html += "]);";
  html += "var optionsTemp = {";
  html += "width: 200, height: 120,";
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
  html += "['湿度', " + String(humidity) + "]";
  html += "]);";
  html += "var optionsHumidity = {";
  html += "width: 200, height: 120,";
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
  html += "}";

  html += "</script>";
  html += "</head><body>";
  html += "<h2>温度和湿度</h2>";
  html += "<div id=\"chart_temp_div\"></div>";
  html += "<div id=\"chart_humidity_div\"></div>";
  html += "<p>温度：" + String(temperature, 1) + "\u00B0C</p>";
  html += "<p>湿度：" + String(humidity, 0) + "%</p>";
  html += "<p>WiFi SSID：" + WiFi.SSID() + "</p>";
  html += "<p>IP地址：" + WiFi.localIP().toString() + "</p>";
  html += "<p>RSSI：" + String(WiFi.RSSI()) + "db</p>";
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

//如果被墙可以用这个，使用百度ECharts JS库，仪表盘在电脑页面大小显示不正常
/*void dht22()  
{
  if (server.uri() != "/dht22")
  {
    server.send(404, "text/plain", "Not Found");
    return;
  }
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  String html = "<html><head><meta charset=\"UTF-8\">";
  html += "<script src=\"https://cdn.jsdelivr.net/npm/echarts@5.0.2/dist/echarts.min.js\"></script>";
  html += "<script type=\"text/javascript\">";
  html += "function drawChart() {";
  html += "var temperatureChart = echarts.init(document.getElementById('temperature_chart'));";
  html += "var temperatureOption = {";
  html += "series: [{";
  html += "name: '温度',";
  html += "type: 'gauge',";
  html += "center: ['50%', '70%'],";
  html += "radius: '60%',";
  html += "detail: {formatter:'{value}°C'},";
  html += "data: [{value: " + String(temperature) + ", name: '温度'}]";
  html += "}]";
  html += "};";
  html += "temperatureChart.setOption(temperatureOption);";
  html += "var humidityChart = echarts.init(document.getElementById('humidity_chart'));";
  html += "var humidityOption = {";
  html += "series: [{";
  html += "name: '湿度',";
  html += "type: 'gauge',";
  html += "center: ['50%', '70%'],";
  html += "radius: '60%',";
  html += "detail: {formatter:'{value}%'},";
  html += "data: [{value: " + String(humidity) + ", name: '湿度'}]";
  html += "}]";
  html += "};";
  html += "humidityChart.setOption(humidityOption);";
  html += "}";
  html += "document.addEventListener('DOMContentLoaded', function() {";
  html += "drawChart();";
  html += "});";
  html += "</script>";
  html += "<style>";
  html += "html, body {";
  html += "height: 100%;";
  html += "display: flex;";
  html += "flex-direction: column;";
  html += "justify-content: center;";
  html += "align-items: center;";
  html += "}";
  html += "</style>";
  html += "</head><body>";
  html += "<h2>Blynk版本DHT22双显示屏小时钟WEB诊断页面</h2>";
  html += "<div id=\"temperature_chart\" style=\"width: 300px; height: 300px;\"></div>";
  html += "<div id=\"humidity_chart\" style=\"width: 300px; height: 300px;\"></div>";
  html += "<h2>温度和湿度</h2>";
  html += "<p>温度：" + String(temperature) + "\u00B0C</p>";
  html += "<p>湿度：" + String(humidity) + "%</p>";
  html += "<p>WiFi SSID：" + WiFi.SSID() + "</p>";
  html += "<p>IP地址：" + WiFi.localIP().toString() + "</p>";
  html += "<p>RSSI：" + String(WiFi.RSSI()) + "db</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}*/