# D1 Mini（ESP8266）DHT22双显示屏时钟温度计 V2

基于[MiniWiFiManager](https://github.com/chrisxs/Blynk_Projects/tree/main/MiniWiFiManager_Blynk_Version)写的程序，Web界面功能：OTA、AP配网、Blynk设置

## 使用方法

1. 上传代码后第一次开机先做一次恢复设置：


**防止没清理干净WiFi记录引起无法正常开机**

```
/*
EEPROM Clear

Sets all of the bytes of the EEPROM to 0.
This example code is in the public domain.

*/

#include <EEPROM.h>

void setup() {
EEPROM.begin(512);
// write a 0 to all 512 bytes of the EEPROM
for (int i = 0; i < 512; i++) { EEPROM.write(i, 0); }

// turn the LED on when we're done
pinMode(13, OUTPUT);
digitalWrite(13, HIGH);
EEPROM.end();
}

void loop() {}
```

## WiFi配置使用方法

1. 写入程序后会新建一个名为：`双显示屏时钟-DEMO`的WiFi热点

2. 进入该热点后配置WiFi即可

3. 预览：

![](https://github.com/chrisxs/Arduino_Cloud/blob/main/MiniWiFiManager/demo.png)

4. 连接过WiFi后，随时可以更改WiFi连接信息。

## OTA方法：

在AP模式或者在连接WiFi后，根据页面提示，在项目目录路径下的：`.pio\build\d1_mini`找到`firmware.bin`二进制文件上传即可，也可以进入`ip:8080`

## Blynk配置方法

1. 首选连接好WiFi，点击`已连接WiFi的，点击此处配置Blynk`（AP模式时候不可用,但可以进入`ip:8080`）

2. 浏览器地址栏输入本设备的`ip:8080`

3. 按照指示做即可

**已保存的Blynk的信息不会被删除隐藏，用于方便校对信息和排查**

### 以上页面除了OTA页面外，都需要账号密码验证，通过修改文件更改密码,不需要密码可以直接注释掉：
- 修改`wifi_setting.h`中的：

```
{
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
  ```

- 修改`blynk_config.h`中的:
```
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
  ```

## 使用库：
```
d1_mini dependencies...
Platform espressif8266 @ 4.2.0 (required: espressif8266)
├── framework-arduinoespressif8266 @ 3.30102.0 (required: platformio/framework-arduinoespressif8266 @ ~3.30102.0)
├── tool-esptool @ 1.413.0 (required: platformio/tool-esptool @ <2)
├── tool-esptoolpy @ 1.30000.201119 (required: platformio/tool-esptoolpy @ ~1.30000.0)
├── tool-mklittlefs @ 1.203.210628 (required: platformio/tool-mklittlefs @ ~1.203.0)
├── tool-mkspiffs @ 1.200.0 (required: platformio/tool-mkspiffs @ ~1.200.0)
└── toolchain-xtensa @ 2.100300.220621 (required: platformio/toolchain-xtensa @ ~2.100300.0)

Libraries
├── Adafruit Unified Sensor @ 1.1.9 (required: adafruit/Adafruit Unified Sensor @ 1.1.9)
├── Blynk @ 1.2.0 (required: blynkkk/Blynk @ 1.2.0)
├── DHT sensor library @ 1.4.4 (required: adafruit/DHT sensor library @ 1.4.4)
├── ESP8266 and ESP32 OLED driver for SSD1306 displays @ 4.4.0 (required: thingpulse/ESP8266 and ESP32 OLED driver for SSD1306 displays @ 4.4.0)
└── ElegantOTA @ 2.2.9 (required: ayushsharma82/ElegantOTA @ 2.2.9)
 *  终端将被任务重用，按任意键关闭。 

 *  正在执行任务: C:\Users\chrisxs\.platformio\penv\Scripts\platformio.exe pkg list --environment d1_mini 

Resolving d1_mini dependencies...
Platform espressif8266 @ 4.2.0 (required: espressif8266)
├── framework-arduinoespressif8266 @ 3.30102.0 (required: platformio/framework-arduinoespressif8266 @ ~3.30102.0)
├── tool-esptool @ 1.413.0 (required: platformio/tool-esptool @ <2)
├── tool-esptoolpy @ 1.30000.201119 (required: platformio/tool-esptoolpy @ ~1.30000.0)
├── tool-mklittlefs @ 1.203.210628 (required: platformio/tool-mklittlefs @ ~1.203.0)
├── tool-mkspiffs @ 1.200.0 (required: platformio/tool-mkspiffs @ ~1.200.0)
└── toolchain-xtensa @ 2.100300.220621 (required: platformio/toolchain-xtensa @ ~2.100300.0)

Libraries
├── Adafruit Unified Sensor @ 1.1.9 (required: adafruit/Adafruit Unified Sensor @ 1.1.9)
├── Blynk @ 1.2.0 (required: blynkkk/Blynk @ 1.2.0)
├── DHT sensor library @ 1.4.4 (required: adafruit/DHT sensor library @ 1.4.4)
├── ESP8266 and ESP32 OLED driver for SSD1306 displays @ 4.4.0 (required: thingpulse/ESP8266 and ESP32 OLED driver for SSD1306 displays @ 4.4.0)
└── ElegantOTA @ 2.2.9 (required: ayushsharma82/ElegantOTA @ 2.2.9)
```
## 更新日志：
2023年5月6日
- 增加Web页面访问密码功能
---
2023年5月5日
- 上传代码
---
