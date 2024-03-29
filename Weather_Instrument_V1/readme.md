# D1 Mini（ESP8266）4合1气象采集仪 V1

## 使用传感器
- 温湿度：si7201
- 光照度：bh1750fvi
- 大气压：bmp180

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

3. 在代码中分别设置好以下内容：
```
char auth[] = "你的Blynk token";
char ssid[] = "你的WiFi_SSID";
char pass[] = "你的WiFi密码";
char blynk_server[] = "Blynk服务器路径";
int blynk_port = 8080; //Blynk端口号
```

## OTA方法：
1. 在代码上传过并成功连接WiFi后，编译程序
2. 在显示屏或者串口查看本机IP，浏览器输入该IP地址：`ip地址:8266/update`,选择`Firmware`然后选择文件
3. 然后在项目目录路径下的：`.pio\build\d1_mini`找到`firmware.bin`二进制文件即可

## WEB串口：
- 在显示屏或者串口查看本机IP，浏览器输入该IP地址：ip地址:340/webserial
- 串口文本框中输入`debug`，然后点击`send`，即可显示WiFi_SSID等相关信息

## 使用库：
```
d1_mini dependencies...
Platform espressif8266 @ 2.6.3 (required: espressif8266)
├── framework-arduinoespressif8266 @ 3.20704.0 (required: platformio/framework-arduinoespressif8266 @ ~3.20704.0)
├── tool-esptool @ 1.413.0 (required: platformio/tool-esptool @ <2)
├── tool-esptoolpy @ 1.30000.201119 (required: platformio/tool-esptoolpy @ ~1.30000.0)
├── tool-mklittlefs @ 1.203.210628 (required: platformio/tool-mklittlefs @ ~1.203.0)
├── tool-mkspiffs @ 1.200.0 (required: platformio/tool-mkspiffs @ ~1.200.0)
└── toolchain-xtensa @ 2.40802.200502 (required: platformio/toolchain-xtensa @ ~2.40802.0)

Libraries
├── Adafruit BMP085 Library @ 1.2.2 (required: adafruit/Adafruit BMP085 Library @ ^1.2.2)
│   ├── Adafruit BusIO @ 1.14.1 (required: Adafruit BusIO)
│   └── Adafruit Unified Sensor @ 1.1.9 (required: Adafruit Unified Sensor)
├── Adafruit Si7021 Library @ 1.5.1 (required: adafruit/Adafruit Si7021 Library @ ^1.5.1)
├── AsyncElegantOTA @ 2.2.7 (required: ayushsharma82/AsyncElegantOTA)
├── Blynk @ 1.2.0 (required: blynkkk/Blynk @ ^1.2.0)
├── ESP Async WebServer @ 1.2.3 (required: ESP Async WebServer)
│   ├── AsyncTCP @ 1.1.1 (required: AsyncTCP)
│   └── ESPAsyncTCP @ 1.2.2 (required: ESPAsyncTCP)
└── WebSerial @ 1.3.0 (required: ayushsharma82/WebSerial)
```
## 更新日志：
2023年3月25日
- 统一从ArduinoIDE转移到PlatformIO
- 增加WEB OTA功能
- 增加Web串口功能

