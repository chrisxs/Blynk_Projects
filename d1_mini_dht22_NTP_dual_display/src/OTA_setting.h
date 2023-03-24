#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

AsyncWebServer server(8266);
void OTA();

void OTA() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<html><head><meta charset='UTF-8'></head><body>ESP8266 OTA界面</body></html>";
    request->send(200, "text/html", html);
  });
  AsyncElegantOTA.begin(&server);
  server.begin();
}
