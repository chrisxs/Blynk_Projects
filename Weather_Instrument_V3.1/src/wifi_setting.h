#include <ESP8266WebServer.h>

const char *ssid = "ESP-DEMO"; // 创建一个热点，热点名称为"ESP8266AP"
const char *password = "";      // 热点密码为空

ESP8266WebServer server(80); // 创建一个Web服务器，端口号为80

void connectWiFi();   // 声明连接WiFi的函数
void handleRoot();    // 声明处理根目录请求的函数
void handleSave();    // 声明处理保存WiFi凭据请求的函数
void handleRestart(); // 声明处理重启设备请求的函数
void handleClear();   // 声明处理清除WiFi凭据请求的函数
bool loadConfig();    // 声明读取WiFi凭据的函数


void handleRoot() {//字串符模板
  const char *http_username = "admin";
  const char *http_password = "admin";

  if (!server.authenticate(http_username, http_password))
  {
    server.requestAuthentication();
    return;
  }

  String html = R"HTML(
    <html>
    <head>
      <meta charset="UTF-8">
      <style> body { text-align: center; } </style>
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>ESP8266 WiFi连接配置页面</title>
    </head>
    <body>
      <h1>4和1气象采集仪 V 3.1 MiniWiFiManager</h1>
      <p>主页：chrisxs.com</p><br><br>
      <img src="https://chrisxs.com/web-images/iot.png" style="width:20%;" /><br><br>
      <a href="http://{LOCAL_IP}:8266/update">已连接WiFi的，点击此处更新固件</a><br><br>
      <a href="http://{LOCAL_IP}:8080">已连接WiFi的，点击此处配置Blynk</a><br><br>
      <a href="http://192.168.4.1:8266/update">未连接WiFi的，点击此处更新固件</a><br><br>
      <form method="post" action="/save">
        <label>WiFi SSID:</label>
        <input type="text" name="ssid"><br>
        <label>WiFi 密码:</label>
        <input type="password" name="password"><br>
        <input type="submit" value="保存"><br>
        <input type="button" value="重启" onclick="restart()">
        <input type="button" value="清除WiFi凭据" onclick="clearCred()">
      </form>
      <script>
        function restart() {
          var xhr = new XMLHttpRequest();
          xhr.open("POST", "/restart", true);
          xhr.onload = function() {
            if (xhr.readyState === xhr.DONE) {
              if (xhr.status === 200) {
                alert('重启成功！');
              } else {
                alert('重启失败！');
              }
            }
          };
          xhr.send();
        }

        function clearCred() {
          var xhr = new XMLHttpRequest();
          xhr.open("POST", "/clear", true);
          xhr.onload = function() {
            if (xhr.readyState === xhr.DONE) {
              if (xhr.status === 200) {
                alert('WiFi凭据清理成功！');
              } else {
                alert('WiFi凭据清理失败！');
              }
            }
          };
          xhr.send();
        }
      </script>
    </body>
    </html>
  )HTML";

  html.replace("{LOCAL_IP}", WiFi.localIP().toString());

  server.send(200, "text/html", html);
}


/*void handleRoot() {
    // 获取用户名和密码
  const char *http_username = "admin";
  const char *http_password = "admin";

  // 检查是否提供了用户名和密码
  if (!server.authenticate(http_username, http_password))
  {
    // 如果没有提供用户名和密码，则发送身份验证失败的响应
    server.requestAuthentication();
    return;
  }

  // 授权通过，继续处理请求
  String html = "<html><head>"
                "<meta charset=\"UTF-8\">"
                "<style> body { text-align: center; } </style>"
                "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
                "<title>ESP8266 WiFi连接配置页面</title>"
                "<h1>滑稽哥的mini WiFiManager!</h1>" // 标题
                "<p>主页：chrisxs.com</p><br><br>" // 网站链接
                "<img src='https://chrisxs.com/web-images/iot.png' style=\"width:20%;\" /><br><br>" // 图片链接，可有可无，方便用户诊断是否已经连接到公网
                "<a href='http://" + WiFi.localIP().toString() + ":8266/update'>已连接WiFi的，点击此处更新固件</a><br><br>" // 连接WiFi后更新固件链接
                "<a href='http://" + WiFi.localIP().toString() + ":8080'>已连接WiFi的，点击此处配置Blynk</a><br><br>" // 连接WiFi后更新固件链接
                "<a href='http://192.168.4.1:8266/update'>未连接WiFi的，点击此处更新固件</a><br><br>" // 未连接WiFi更新固件链接
                "</head><body>"
                "<form method='post' action='/save'>" // 保存WiFi名称和密码
                "<label>WiFi SSID:</label>"
                "<input type='text' name='ssid'><br>"
                "<label>WiFi 密码:</label>"
                "<input type='password' name='password'><br>"
                "<input type='submit' value='保存'><br>"
                "<input type='button' value='重启' onclick='restart()'>" // 重启按钮，点击后调用 restart() 函数
                "<input type='button' value='清除WiFi凭据' onclick='clearCred()'>" // 清除WiFi凭据按钮，点击后调用 clearCred() 函数
                "</form>"
                "<script>"
                "function restart() {" // 重启函数
                "var xhr = new XMLHttpRequest();"
                "xhr.open(\"POST\", \"/restart\", true);"
                "xhr.onload = function() {"
                "if (xhr.readyState === xhr.DONE) {"
                "if (xhr.status === 200) {"
                "alert('重启成功！');" // 弹出成功提示
                "} else {"
                "alert('重启失败！');" // 弹出失败提示
                "}"
                "}"
                "};"
                "xhr.send();"
                "}"
                "function clearCred() {" // 清除WiFi凭据函数
                "var xhr = new XMLHttpRequest();"
                "xhr.open(\"POST\", \"/clear\", true);"
                "xhr.onload = function() {"
                "if (xhr.readyState === xhr.DONE) {"
                "if (xhr.status === 200) {"
                "alert('WiFi凭据清理成功！');" // 弹出成功提示
                "} else {"
                "alert('WiFi凭据清理失败！');" // 弹出失败提示
                "}"
                "}"
                "};"
                "xhr.send();"
                "}"
                "</script>"
                "</body></html>";
  server.send(200, "text/html", html); // 发送html页面到客户端
}*/


// 处理保存WiFi连接信息的请求
void handleSave()
{
  String ssid = server.arg("ssid");
  String password = server.arg("password");
  if (server.hasArg("clear"))
  {
    // 如果点击了“Clear WiFi Credentials”按钮，则删除配置文件并重启设备。
    SPIFFS.remove("/config.txt");
    ESP.restart();
  }
  // else if (ssid.length() > 0) //允许连接无密码的WiFi_SSID（不建议，太不安全）
  else if (ssid.length() > 0 && password.length() > 0) // 不允许连接无密码的WiFi_SSID
  {
    // 如果输入了新的SSID和密码，则保存它们并重启设备。
    File configFile = SPIFFS.open("/config.txt", "w");
    if (configFile)
    {
      configFile.println(ssid);
      configFile.println(password);
      configFile.close();
      Serial.println("WiFi凭据保存成功");
      Serial.println("WiFi_SSID： " + ssid);
      Serial.println("WiFi_密码： " + password);
      server.send(200, "text/html", "<meta charset=\"UTF-8\"><p>保存成功！！稍后即将重启！</p>");
      delay(3000);
      ESP.restart();
      return;
    }
  }
  server.send(400, "text/html", "<meta charset=\"UTF-8\"><p>保存失败！！请返回上个页面！</p>");
}

// 从 SPIFFS 文件系统中读取 WiFi 配置
bool loadConfig()
{
  if (SPIFFS.exists("/config.txt"))
  {
    // 如果存在配置文件，则打开它并读取 SSID 和密码。
    File configFile = SPIFFS.open("/config.txt", "r");
    if (configFile)
    {
      String ssid = configFile.readStringUntil('\n');
      String password = configFile.readStringUntil('\n');
      configFile.close();
      ssid.trim();
      password.trim();
      // 如果读取到有效的 SSID 和密码，则连接 WiFi。
      if (ssid.length() > 0 && password.length() > 0)
      {
        WiFi.begin(ssid.c_str(), password.c_str());
        int tries = 0;
        Serial.print("正在连接  ");
        Serial.println(ssid);
        while (WiFi.status() != WL_CONNECTED && tries < 10)
        {
          delay(1000);
          Serial.print(".");
          tries++;
        }
        // 如果成功连接 WiFi，则打印 IP 地址并返回 true。
        if (WiFi.status() == WL_CONNECTED)
        {
          Serial.println("");
          Serial.print("WiFi 已经连接, IP 地址: ");
          Serial.println(WiFi.localIP());
          return true;
        }
        // 如果连接失败，则打印错误信息并返回 false。
        else
        {
          Serial.println("");
          Serial.println("连接 WiFi 失败");
        }
      }
    }
  }
  // 如果没有找到配置文件，则返回 false。
  return false;
}

// 连接 WiFi，如果已连接，则不执行任何操作
void connectWiFi()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    return;
  }
  Serial.println("正在连接 WiFi...");
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 10)
  {
    delay(1000);
    Serial.print(".");
    tries++;
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("");
    Serial.print("WiFi 已经连接, IP 地址: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    WiFi.disconnect(); // 断开 WiFi 连接
    delay(1000);
    ESP.restart(); // 重启设备
  }
}

// 处理重启请求
void handleRestart()
{
  server.send(200, "text/plain", "Restarting..."); // 发送响应
  delay(1000);
  Serial.print("用户点击重启");
  delay(3000);
  ESP.restart(); // 重启设备
}

// 处理清除 WiFi 凭据请求
void handleClear()
{
  SPIFFS.remove("/config.txt");                        // 删除凭据文件
  server.send(200, "text/plain", "Clear Credentials"); // 发送响应
  Serial.print("用户点击清除WiFi凭据，设备将会重启");
  delay(3000);
  WiFi.disconnect(); // 断开 WiFi 连接
  delay(1000);
  ESP.restart(); // 重启设备
}