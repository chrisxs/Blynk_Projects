// #include <ESP8266WiFi.h>
// #include <ESP8266WebServer.h>
// #include <FS.h>
#include <BlynkSimpleEsp8266.h>

ESP8266WebServer blynk_config_server(8080);

// Default values for Blynk configuration
String blynk_token;
String blynk_server;
int blynk_port;

void blynk_config_server_handleRoot()
{
    blynk_config_server.send(200, "text/html", "<html><head>"
                                               "<meta charset=\"UTF-8\">"
                                               "<style> body { text-align: center; } </style>"
                                               "<form action='/config' method='POST'>"
                                               "<label for='token'>Blynk Token:</label><br>"
                                               "<input type='text' id='token' name='token' value='" +
                                                   blynk_token + "'><br>"
                                                                 "<label for='server'>Blynk Server:</label><br>"
                                                                 "<input type='text' id='server' name='server' value='" +
                                                   blynk_server + "'><br>"
                                                                  "<label for='port'>Blynk Port:</label><br>"
                                                                  "<input type='text' id='port' name='port' value='" +
                                                   String(blynk_port) + "'><br><br>"
                                                                        "<input type='submit' value='Save'>"
                                                                        "</form><br>"
                                                                        "<form action='/clear' method='POST'>"
                                                                        "<input type='submit' value='Clear Configuration'>"
                                                                        "</form>");
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
            Serial.println("Failed to open config file for writing");
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
        delay(3000);
        ESP.restart();
    }
    else
    {
        Serial.println("保存失败，你输入的是：");
        Serial.println(blynk_token);
        Serial.println(blynk_server);
        Serial.println(blynk_port);
        blynk_config_server.send(400, "text/plain", "Invalid Request");
    }
}

void blynk_config_server_handleClear()
{
    if (blynk_config_server.method() == HTTP_POST)
    {
        SPIFFS.remove("/blynk.txt");
        blynk_config_server.send(200, "text/html", "<meta charset=\"UTF-8\"><p>Blynk配置已经清除！稍后将会重启！</p>");
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
    // Serial.println(blynk_token);
    // Serial.println(blynk_server);
    // Serial.println(blynk_port);
    if (configFile)
    {
        blynk_token = configFile.readStringUntil('\n');
        // blynk_token.trim();
        blynk_server = configFile.readStringUntil('\n');
        // blynk_server.trim();
        blynk_port = configFile.readStringUntil('\n').toInt();
        configFile.close();
        Serial.println("提取自blynk.txt:");
        Serial.println("Blynk Token: " + blynk_token + ".");
        Serial.println("Blynk Server: " + blynk_server + ".");
        Serial.println("Blyn Port: " + String(blynk_port));
    }
    else
    {
        Serial.println("blynk.txt不存在，无法提取内容");
    }
    Serial.println("用以下参数连接Blynk：");
    Serial.println("Blynk Token: " + blynk_token + ".");
    Serial.println("Blynk Server: " + blynk_server + ".");
    Serial.println("Blyn Port: " + String(blynk_port));
    // Blynk.config(blynk_token.c_str(), blynk_server.c_str(), blynk_port);

    // Blynk.config(blynk_token.c_str(), "chrisxs.com", blynk_port);

    // blynk_config_server.on("/", blynk_config_server_handleRoot);
    // blynk_config_server.on("/config", blynk_config_server_handleConfig);
    // blynk_config_server.on("/clear", blynk_config_server_handleClear);

    // blynk_config_server.begin();
    Serial.println("HTTP server started");
}
