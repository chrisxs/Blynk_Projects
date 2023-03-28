#include <ElegantOTA.h>

ESP8266WebServer OTAserver(8266);

void OTA();

void OTA()
{
    OTAserver.on("/", []()
              { OTAserver.send(200, "text/html", "<meta charset=\"UTF-8\"><p>ESP8266 OTA 界面</p>"); });

    ElegantOTA.begin(&OTAserver); 
    OTAserver.begin();
    Serial.println("OTA服务启动成功");
}