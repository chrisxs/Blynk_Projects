#include <BlynkSimpleEsp8266.h>

ESP8266WebServer blynk_config_server(8080);

String blynk_token;
String blynk_server;
int blynk_port;

const char *http_username = "admin"; // 设置配置页面的账号名称
const char *http_password = "admin"; // 设置配置页面的密码

const int MosPin = 15;
WidgetTerminal terminal(V10);

/////磁力锁，点动后断电开锁10秒，10秒后上电闭锁/////
BLYNK_WRITE(V0)
{
  // int buttonState = param.asInt();
  if (param.asInt() == 1)
  {
    // turn LED on:
    digitalWrite(MosPin, LOW);
    Blynk.virtualWrite(V0, 1);
    delay(5000);
    digitalWrite(MosPin, HIGH);
    Blynk.virtualWrite(V0, 0);
  }
}

/////磁力锁，手动按钮通断电/////
/*BLYNK_WRITE(V0)
{
  // int buttonState = param.asInt();
  if (param.asInt() == 1)
  {
    // turn LED on:
    digitalWrite(MosPin, LOW);
  }
  else
  {
    digitalWrite(MosPin, HIGH);
  }
}*/

/////电插锁，点动后上电开锁10秒，10秒后断电/////
/*BLYNK_WRITE(V0) {
  //int buttonState = param.asInt();
  if (param.asInt() == 1) {
    // turn LED on:
    digitalWrite(MosPin, HIGH);
    delay(5000);
    digitalWrite(MosPin, LOW);
  }
  }*/

BLYNK_CONNECTED()
{
  Blynk.syncAll();
  digitalWrite(2, LOW);
}

BLYNK_WRITE(V10)
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
  terminal.flush();
}

void blynk_config_server_handleRoot()
{
  // const char *http_username = "admin"; // 设置Blynk配置页面的账号名称
  // const char *http_password = "admin"; // 设置Blynk配置页面的密码

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

void handleButton()
{ // 获取用户名和密码
  // const char *http_username = "admin";
  // const char *http_password = "admin";

  // 检查是否提供了用户名和密码
  if (!server.authenticate(http_username, http_password))
  {
    // 如果没有提供用户名和密码，则发送身份验证失败的响应
    server.requestAuthentication();
    return;
  }

  // 获取按钮的值
  String buttonValue = server.arg("button");

  // 根据按钮的值设置引脚输出
  if (buttonValue == "HIGH")
  {
    digitalWrite(MosPin, HIGH);
    Blynk.virtualWrite(V0, 0);
  }
  else if (buttonValue == "LOW")
  {
    digitalWrite(MosPin, LOW);
    Blynk.virtualWrite(V0, 1);
  }

  // 获取当前引脚的电平值
  int currentLevel = digitalRead(MosPin);

  // 设置电平状态文本
  String levelText;
  if (currentLevel == HIGH)
  {
    levelText = "高";
  }
  else
  {
    levelText = "低";
  }

  // 返回Web页面
  String html = "<html><head><meta charset=\"UTF-8\"></head><body>";
  html += "<center><h1>手动应急按钮</h1>";
  html += "<h2>（仅供断网应急用）</h2>";
  html += "<form method=\"get\" action=\"/button\">";
  html += "<button name=\"button\" value=\"HIGH\" type=\"submit\">高电平</button>";
  html += "<button name=\"button\" value=\"LOW\" type=\"submit\">低电平</button>";
  html += "<h2>设备信息</h2>";
  html += "<p>WiFi SSID：" + WiFi.SSID() + "</p>";
  html += "<p>IP地址：" + WiFi.localIP().toString() + "</p>";
  html += "<p>MAC地址：" + String(WiFi.macAddress()) + "</p>";
  html += "<p>RSSI：" + String(WiFi.RSSI()) + "db</p>";
  html += "<h2>当前电平值</h2>";
  // html += "<p>电平：" + String(currentLevel) + "</p>";//显示纯数值的电平状态
  html += "<p>电平：" + levelText + "</p>"; // 显示中文的电平状态文本
  html += "</form></body></html></center>";
  server.send(200, "text/html", html);
}
