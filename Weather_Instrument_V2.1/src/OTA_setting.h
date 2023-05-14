#include <ESPAsyncTCP.h>       // 引入 ESPAsyncTCP 库，用于处理 TCP/IP 协议
#include <ESPAsyncWebServer.h> // 引入 ESPAsyncWebServer 库，用于处理 Web 服务器
#include <AsyncElegantOTA.h>   // 引入 AsyncElegantOTA 库，用于处理 OTA（空中固件升级）功能

#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

AsyncWebServer server(8266); // 创建 AsyncWebServer 实例，监听 8266 端口
void OTA();                  // 定义 OTA 函数

void OTA()
{
  // 处理根目录请求
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              String html = "<html><head><meta charset='UTF-8'></head><body>ESP8266 OTA界面</body></html>";
              request->send(200, "text/html", html); // 发送 HTTP 响应
            });
  AsyncElegantOTA.begin(&server); // 启动 OTA 功能
  server.begin();                 // 启动 Web 服务器
}