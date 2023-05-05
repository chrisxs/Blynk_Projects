#include <BlynkSimpleEsp8266.h>

ESP8266WebServer blynk_config_server(8080);

// Default values for Blynk configuration
String blynk_token;
String blynk_server;
int blynk_port;

//String ntp_server;

char ntp_server[] = "time.windows.com"; // NTP服务器

BlynkTimer timer;

#define DHTPIN D2
#define DHTTYPE DHT22

BLYNK_CONNECTED()
{
    Blynk.syncVirtual(V5); // 在连接 Blynk 服务器后同步虚拟引脚 V5
}

void blynk_config_server_handleRoot()
{
    String html = "<html><head><meta charset=\"UTF-8\"><style> body { text-align: center; } </style></head><body>";
    html += "<h1>Blynk配置界面!</h1>";
    html += "<form action='/config' method='POST'>";
    html += "<label for='token'>Blynk Token</label><br>";
    html += "<input type='text' id='token' name='token' value='" + blynk_token + "'><br>";
    html += "<label for='server'>Blynk 服务器URL</label><br>";
    html += "<input type='text' id='server' name='server' value='" + blynk_server + "'><br>";
    html += "<label for='port'>Blynk 端口号</label><br>";
    html += "<input type='text' id='port' name='port' value='" + String(blynk_port) + "'><br><br>";
    //html += "<label for='ntp_server'>NTP 服务器</label><br>";
    //html += "<input type='text' id='ntp_server' name='ntp_server' value='" + ntp_server + "'><br><br>";
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
        //ntp_server = blynk_config_server.arg("ntp_server");
        File configFile = SPIFFS.open("/blynk.txt", "w");
        if (!configFile)
        {
            Serial.println("文件写入失败");
            Serial.println();
        }
        configFile.printf("%s\n", blynk_token.c_str());
        configFile.printf("%s\n", blynk_server.c_str());
        configFile.printf("%d\n", blynk_port);
        //configFile.printf("%d\n", ntp_server.c_str());
        configFile.close();
        blynk_config_server.send(200, "text/html", "<meta charset=\"UTF-8\"><p>Blynk配置保存成功！稍后即将重启！</p>");
        Serial.println("保存成功，你输入的是：");
        Serial.println(blynk_token);
        Serial.println(blynk_server);
        Serial.println(blynk_port);
        //Serial.println(ntp_server);
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
        //Serial.println(ntp_server);
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
        //ntp_server = configFile.readStringUntil('\n');
        configFile.close();
        Serial.println("读取blynk.txt成功，文件中的内容如下:");
        Serial.println(blynk_token);
        Serial.println(blynk_server);
        Serial.println(String(blynk_port));
        //Serial.println(ntp_server);
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
    //Serial.println("NTP Server: " + ntp_server);
    Serial.println();
}
