#include <BlynkSimpleEsp8266.h>

ESP8266WebServer blynk_config_server(8080);

String blynk_token;
String blynk_server;
int blynk_port;


void blynk_config_server_handleRoot()
{
  const char *http_username = "admin";
  const char *http_password = "admin";

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
