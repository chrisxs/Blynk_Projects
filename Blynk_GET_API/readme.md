# 使用使用ESP8266对Blynk服务器送请求，通过反馈的JSON内容再显示在ESP8266上

## 使用库：
- thingpulse/ESP8266 and ESP32 OLED driver for SSD1306 - displays @ ^4.3.0
- tzapu/WiFiManager@0.16.0
- bblanchon/ArduinoJson@6.17.3

## 更新日志：

2023年3月20日
- 改成使用WiFiManager，使用AP模式连接ESP8266，输入相关项目和选择连接WiFi

- Blynk的可选填入项目：
    - 服务器URL
    - 服务器端口
    - 密钥

- 加入基于IDE的OTA功能（准备弃用）

- 精简部分代码

---

2023年3月19日
- 上传
