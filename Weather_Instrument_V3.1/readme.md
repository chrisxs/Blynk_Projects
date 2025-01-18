# D1 Mini（ESP8266）4合1气象采集仪 V3

## 使用传感器
- 温湿度：SHT31
- 光照度：BH1750FVI
- 大气压：BMP085

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

~~2. 按下重置按钮`D5`，板载LED会亮起并闪耀，等待约30秒~~
3. 用手机或者电脑连接名为：`ESP8266AP`的WiFi热点，浏览器进入`192.168.4.1`，配置好里面的WiFi和Blynk相关信息即可

~~**目前Web OTA和Web 串口会跟WiFiManager有冲突，所以未加入**~~
## OTA方法：
1. 在代码上传过并成功连接WiFi后，编译程序
2. 在显示屏或者串口查看本机IP，浏览器输入该IP地址：`ip地址:8266/update`,选择`Firmware`然后选择文件
3. 然后在项目目录路径下的：`.pio\build\d1_mini`找到`firmware.bin`二进制文件即可

~~## WEB串口：~~


## 使用库：
```
d1_mini dependencies...
Platform espressif8266 @ 2.6.3 (required: espressif8266 @ 2.6.3)
├── framework-arduinoespressif8266 @ 3.20704.0 (required: platformio/framework-arduinoespressif8266 @ ~3.20704.0)
├── tool-esptool @ 1.413.0 (required: platformio/tool-esptool @ <2)
├── tool-esptoolpy @ 1.30000.201119 (required: platformio/tool-esptoolpy @ ~1.30000.0)
├── tool-mklittlefs @ 1.203.210628 (required: platformio/tool-mklittlefs @ ~1.203.0)
├── tool-mkspiffs @ 1.200.0 (required: platformio/tool-mkspiffs @ ~1.200.0)
└── toolchain-xtensa @ 1.40802.0 (required: platformio/toolchain-xtensa @ ~1.40802.0)

Libraries
├── Adafruit BMP085 Library @ 1.2.2 (required: adafruit/Adafruit BMP085 Library @ 1.2.2)
│   ├── Adafruit BusIO @ 1.17.0 (required: Adafruit BusIO)
│   └── Adafruit Unified Sensor @ 1.1.15 (required: Adafruit Unified Sensor)
├── ArduinoJson @ 6.21.0 (required: bblanchon/ArduinoJson @ 6.21.0)
├── BH1750FVI @ 1.1.1 (required: peterembedded/BH1750FVI @ 1.1.1)
├── Blynk @ 1.2.0 (required: blynkkk/Blynk @ 1.2.0)
├── ElegantOTA @ 2.2.9 (required: ayushsharma82/ElegantOTA @ 2.2.9)
├── WEMOS SHT3x @ 1.0.0 (required: wemos/WEMOS SHT3x @ 1.0.0)
└── WiFiManager @ 0.16.0 (required: tzapu/WiFiManager @ 0.16.0)
```
## 更新日志：
2025年1月18日
- 改用mini WiFiManage，弃用旧版用按键配网的方式
- 增加Web诊断页面，显示WiFi信息，同时显示传感器数值，方便诊断输入：`ip//sensor_web_page`进入
- 增加在blynk端的终端插件显示错误信息
- 增加在blynk端的终端可用的命令,分别为:`wifi`显示WiFi信息，`blynk`显示blynk信息，`clear`情况终端所有显示内容
---
2023年3月27日
- 增加错误处理，防止在传感器松脱的情况下程序死机
---
2023年3月25日
- 统一从ArduinoIDE转移到PlatformIO


